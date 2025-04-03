}
static void OutputFrame( sout_stream_t *p_stream, picture_t *p_pic, sout_stream_id_sys_t *id, block_t **out )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    picture_t *p_pic2 = NULL;
    const mtime_t original_date = p_pic->date;
    bool b_need_duplicate=false;
    /* If input pts is lower than next_output_pts - output_frame_interval
     * Then the future input frame should fit better and we can drop this one 
     *
     * We check it here also because we can have case that video filters outputs multiple
     * pictures but we don't need to use them all, for example yadif2x and outputting to some
     * different fps value
     */
    if( ( original_date ) <
        ( date_Get( &id->next_output_pts ) - (mtime_t)id->i_output_frame_interval ) )
    {
#if 0
        msg_Dbg( p_stream, "dropping frame (%"PRId64" + %"PRId64" vs %"PRId64")",
                 p_pic->date, id->i_input_frame_interval, date_Get(&id->next_output_pts) );
#endif
        picture_Release( p_pic );
        return;
    }
    /*
     * Encoding
     */
    /* Check if we have a subpicture to overlay */
    if( p_sys->p_spu )
    {
        video_format_t fmt = id->p_encoder->fmt_in.video;
        if( fmt.i_visible_width <= 0 || fmt.i_visible_height <= 0 )
        {
            fmt.i_visible_width  = fmt.i_width;
            fmt.i_visible_height = fmt.i_height;
            fmt.i_x_offset       = 0;
            fmt.i_y_offset       = 0;
        }
        subpicture_t *p_subpic = spu_Render( p_sys->p_spu, NULL, &fmt, &fmt,
                                             p_pic->date, p_pic->date, false );
        /* Overlay subpicture */
        if( p_subpic )
        {
            if( picture_IsReferenced( p_pic ) && !filter_chain_GetLength( id->p_f_chain ) )
            {
                /* We can't modify the picture, we need to duplicate it,
                 * in this point the picture is already p_encoder->fmt.in format*/
                picture_t *p_tmp = video_new_buffer_encoder( id->p_encoder );
                if( likely( p_tmp ) )
                {
                    picture_Copy( p_tmp, p_pic );
                    picture_Release( p_pic );
                    p_pic = p_tmp;
                }
            }
            if( unlikely( !p_sys->p_spu_blend ) )
                p_sys->p_spu_blend = filter_NewBlend( VLC_OBJECT( p_sys->p_spu ), &fmt );
            if( likely( p_sys->p_spu_blend ) )
                picture_BlendSubpicture( p_pic, p_sys->p_spu_blend, p_subpic );
            subpicture_Delete( p_subpic );
        }
    }
    /* set output pts*/
    p_pic->date = date_Get( &id->next_output_pts );
    /*This pts is handled, increase clock to next one*/
    date_Increment( &id->next_output_pts, id->p_encoder->fmt_in.video.i_frame_rate_base );
    if( p_sys->i_threads == 0 )
    {
        block_t *p_block;
        p_block = id->p_encoder->pf_encode_video( id->p_encoder, p_pic );
        block_ChainAppend( out, p_block );
    }
    /* we need to duplicate while next_output_pts + output_frame_interval < input_pts (next input pts)*/
    b_need_duplicate = ( date_Get( &id->next_output_pts ) + id->i_output_frame_interval ) <
                       ( original_date );
    if( p_sys->i_threads )
    {
        if( p_sys->b_master_sync )
        {
            p_pic2 = video_new_buffer_encoder( id->p_encoder );
            if( likely( p_pic2 != NULL ) )
                picture_Copy( p_pic2, p_pic );
        }
        vlc_mutex_lock( &p_sys->lock_out );
        picture_fifo_Push( p_sys->pp_pics, p_pic );
        vlc_cond_signal( &p_sys->cond );
        vlc_mutex_unlock( &p_sys->lock_out );
    }
    while( (p_sys->b_master_sync && b_need_duplicate ))
    {
        if( p_sys->i_threads >= 1 )
        {
            picture_t *p_tmp = NULL;
            /* We can't modify the picture, we need to duplicate it */
            p_tmp = video_new_buffer_encoder( id->p_encoder );
            if( likely( p_tmp != NULL ) )
            {
                picture_Copy( p_tmp, p_pic2 );
                p_tmp->date = date_Get( &id->next_output_pts );
                vlc_mutex_lock( &p_sys->lock_out );
                picture_fifo_Push( p_sys->pp_pics, p_tmp );
                vlc_cond_signal( &p_sys->cond );
                vlc_mutex_unlock( &p_sys->lock_out );
            }
        }
        else
        {
            block_t *p_block;
            p_pic->date = date_Get( &id->next_output_pts );
            p_block = id->p_encoder->pf_encode_video(id->p_encoder, p_pic);
            block_ChainAppend( out, p_block );
        }
#if 0
        msg_Dbg( p_stream, "duplicated frame");
#endif
        date_Increment( &id->next_output_pts, id->p_encoder->fmt_in.video.i_frame_rate_base );
        b_need_duplicate = ( date_Get( &id->next_output_pts ) + id->i_output_frame_interval ) <
                           ( original_date );
    }
    if( p_sys->i_threads && p_pic2 )
        picture_Release( p_pic2 );
    else if ( p_sys->i_threads == 0 )
        picture_Release( p_pic );
}
