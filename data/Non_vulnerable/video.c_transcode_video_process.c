}
int transcode_video_process( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                                    block_t *in, block_t **out )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    picture_t *p_pic = NULL;
    *out = NULL;
    if( unlikely( in == NULL ) )
    {
        if( p_sys->i_threads == 0 )
        {
            block_t *p_block;
            do {
                p_block = id->p_encoder->pf_encode_video(id->p_encoder, NULL );
                block_ChainAppend( out, p_block );
            } while( p_block );
        }
        else
        {
            msg_Dbg( p_stream, "Flushing thread and waiting that");
            vlc_mutex_lock( &p_stream->p_sys->lock_out );
            p_stream->p_sys->b_abort = true;
            vlc_cond_signal( &p_stream->p_sys->cond );
            vlc_mutex_unlock( &p_stream->p_sys->lock_out );
            vlc_join( p_stream->p_sys->thread, NULL );
            vlc_mutex_lock( &p_sys->lock_out );
            *out = p_sys->p_buffers;
            p_sys->p_buffers = NULL;
            vlc_mutex_unlock( &p_sys->lock_out );
            msg_Dbg( p_stream, "Flushing done");
        }
        return VLC_SUCCESS;
    }
    while( (p_pic = id->p_decoder->pf_decode_video( id->p_decoder, &in )) )
    {
        if( unlikely (
             id->p_encoder->p_module &&
             !video_format_IsSimilar( &id->fmt_input_video, &id->p_decoder->fmt_out.video )
            )
          )
        {
            msg_Info( p_stream, "aspect-ratio changed, reiniting. %i -> %i : %i -> %i.",
                        id->fmt_input_video.i_sar_num, id->p_decoder->fmt_out.video.i_sar_num,
                        id->fmt_input_video.i_sar_den, id->p_decoder->fmt_out.video.i_sar_den
                    );
            /* Close filters */
            if( id->p_f_chain )
                filter_chain_Delete( id->p_f_chain );
            id->p_f_chain = NULL;
            if( id->p_uf_chain )
                filter_chain_Delete( id->p_uf_chain );
            id->p_uf_chain = NULL;
            /* Reinitialize filters */
            id->p_encoder->fmt_out.video.i_visible_width  = p_sys->i_width & ~1;
            id->p_encoder->fmt_out.video.i_visible_height = p_sys->i_height & ~1;
            id->p_encoder->fmt_out.video.i_sar_num = id->p_encoder->fmt_out.video.i_sar_den = 0;
            transcode_video_filter_init( p_stream, id );
            transcode_video_encoder_init( p_stream, id );
            conversion_video_filter_append( id );
            memcpy( &id->fmt_input_video, &id->p_decoder->fmt_out.video, sizeof(video_format_t));
        }
        if( unlikely( !id->p_encoder->p_module ) )
        {
            if( id->p_f_chain )
                filter_chain_Delete( id->p_f_chain );
            if( id->p_uf_chain )
                filter_chain_Delete( id->p_uf_chain );
            id->p_f_chain = id->p_uf_chain = NULL;
            transcode_video_filter_init( p_stream, id );
            transcode_video_encoder_init( p_stream, id );
            conversion_video_filter_append( id );
            memcpy( &id->fmt_input_video, &id->p_decoder->fmt_out.video, sizeof(video_format_t));
            if( transcode_video_encoder_open( p_stream, id ) != VLC_SUCCESS )
            {
                picture_Release( p_pic );
                transcode_video_close( p_stream, id );
                id->b_transcode = false;
                return VLC_EGENERIC;
            }
            date_Set( &id->next_output_pts, p_pic->date );
            date_Set( &id->next_input_pts, p_pic->date );
        }
        /*Input lipsync and drop check */
        if( p_sys->b_master_sync )
        {
            /* If input pts lower than next_output_pts - output_frame_interval
             * Then the future input frame should fit better and we can drop this one 
             *
             * We check this here as we don't need to run video filter at all for pictures
             * we are going to drop anyway
             *
             * Duplication need is checked in OutputFrame */
            if( ( p_pic->date ) <
                ( date_Get( &id->next_output_pts ) - (mtime_t)id->i_output_frame_interval ) )
            {
#if 0
                msg_Dbg( p_stream, "dropping frame (%"PRId64" + %"PRId64" vs %"PRId64")",
                         p_pic->date, id->i_input_frame_interval, date_Get(&id->next_output_pts) );
#endif
                picture_Release( p_pic );
                date_Increment( &id->next_input_pts, id->p_decoder->fmt_out.video.i_frame_rate_base );
                continue;
            }
#if 0
            msg_Dbg( p_stream, "not dropping frame");
#endif
        }
        /* Check input drift regardless, if it's more than 100ms from our approximation, we most likely have lost pictures
         * and are in danger to become out of sync, so better reset timestamps then */
        if( likely( p_pic->date != VLC_TS_INVALID ) )
        {
            mtime_t input_drift = p_pic->date - date_Get( &id->next_input_pts );
            if( unlikely( (input_drift > (CLOCK_FREQ/10)) ||
                          (input_drift < -(CLOCK_FREQ/10))
               ) )
            {
                msg_Warn( p_stream, "Reseting video sync" );
                date_Set( &id->next_output_pts, p_pic->date );
                date_Set( &id->next_input_pts, p_pic->date );
            }
        }
        date_Increment( &id->next_input_pts, id->p_decoder->fmt_out.video.i_frame_rate_base );
        /* Run the filter and output chains; first with the picture,
         * and then with NULL as many times as we need until they
         * stop outputting frames.
         */
        for ( ;; ) {
            picture_t *p_filtered_pic = p_pic;
            /* Run filter chain */
            if( id->p_f_chain )
                p_filtered_pic = filter_chain_VideoFilter( id->p_f_chain, p_filtered_pic );
            if( !p_filtered_pic )
                break;
            for ( ;; ) {
                picture_t *p_user_filtered_pic = p_filtered_pic;
                /* Run user specified filter chain */
                if( id->p_uf_chain )
                    p_user_filtered_pic = filter_chain_VideoFilter( id->p_uf_chain, p_user_filtered_pic );
                if( !p_user_filtered_pic )
                    break;
                OutputFrame( p_stream, p_user_filtered_pic, id, out );
                p_filtered_pic = NULL;
            }
            p_pic = NULL;
        }
    }
    if( p_sys->i_threads >= 1 )
    {
        /* Pick up any return data the encoder thread wants to output. */
        vlc_mutex_lock( &p_sys->lock_out );
        *out = p_sys->p_buffers;
        p_sys->p_buffers = NULL;
        vlc_mutex_unlock( &p_sys->lock_out );
    }
    return VLC_SUCCESS;
}
