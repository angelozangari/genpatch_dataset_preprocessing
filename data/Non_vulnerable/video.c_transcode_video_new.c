}
int transcode_video_new( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    /* Open decoder
     * Initialization of decoder structures
     */
    id->p_decoder->fmt_out = id->p_decoder->fmt_in;
    id->p_decoder->fmt_out.i_extra = 0;
    id->p_decoder->fmt_out.p_extra = 0;
    id->p_decoder->fmt_out.psz_language = NULL;
    id->p_decoder->pf_decode_video = NULL;
    id->p_decoder->pf_get_cc = NULL;
    id->p_decoder->pf_get_cc = 0;
    id->p_decoder->pf_vout_buffer_new = video_new_buffer_decoder;
    id->p_decoder->pf_vout_buffer_del = video_del_buffer_decoder;
    id->p_decoder->pf_picture_link    = video_link_picture_decoder;
    id->p_decoder->pf_picture_unlink  = video_unlink_picture_decoder;
    id->p_decoder->p_owner = malloc( sizeof(decoder_owner_sys_t) );
    if( !id->p_decoder->p_owner )
        return VLC_EGENERIC;
    id->p_decoder->p_owner->p_sys = p_sys;
    /* id->p_decoder->p_cfg = p_sys->p_video_cfg; */
    id->p_decoder->p_module =
        module_need( id->p_decoder, "decoder", "$codec", false );
    if( !id->p_decoder->p_module )
    {
        msg_Err( p_stream, "cannot find video decoder" );
        free( id->p_decoder->p_owner );
        return VLC_EGENERIC;
    }
    /*
     * Open encoder.
     * Because some info about the decoded input will only be available
     * once the first frame is decoded, we actually only test the availability
     * of the encoder here.
     */
    /* Initialization of encoder format structures */
    es_format_Init( &id->p_encoder->fmt_in, id->p_decoder->fmt_in.i_cat,
                    id->p_decoder->fmt_out.i_codec );
    id->p_encoder->fmt_in.video.i_chroma = id->p_decoder->fmt_out.i_codec;
    /* The dimensions will be set properly later on.
     * Just put sensible values so we can test an encoder is available. */
    id->p_encoder->fmt_in.video.i_width =
        id->p_encoder->fmt_out.video.i_width
          ? id->p_encoder->fmt_out.video.i_width
          : id->p_decoder->fmt_in.video.i_width
            ? id->p_decoder->fmt_in.video.i_width : 16;
    id->p_encoder->fmt_in.video.i_height =
        id->p_encoder->fmt_out.video.i_height
          ? id->p_encoder->fmt_out.video.i_height
          : id->p_decoder->fmt_in.video.i_height
            ? id->p_decoder->fmt_in.video.i_height : 16;
    id->p_encoder->fmt_in.video.i_visible_width =
        id->p_encoder->fmt_out.video.i_visible_width
          ? id->p_encoder->fmt_out.video.i_visible_width
          : id->p_decoder->fmt_in.video.i_visible_width
            ? id->p_decoder->fmt_in.video.i_visible_width : id->p_encoder->fmt_in.video.i_width;
    id->p_encoder->fmt_in.video.i_visible_height =
        id->p_encoder->fmt_out.video.i_visible_height
          ? id->p_encoder->fmt_out.video.i_visible_height
          : id->p_decoder->fmt_in.video.i_visible_height
            ? id->p_decoder->fmt_in.video.i_visible_height : id->p_encoder->fmt_in.video.i_height;
    id->p_encoder->i_threads = p_sys->i_threads;
    id->p_encoder->p_cfg = p_sys->p_video_cfg;
    id->p_encoder->p_module =
        module_need( id->p_encoder, "encoder", p_sys->psz_venc, true );
    if( !id->p_encoder->p_module )
    {
        msg_Err( p_stream, "cannot find video encoder (module:%s fourcc:%4.4s). Take a look few lines earlier to see possible reason.",
                 p_sys->psz_venc ? p_sys->psz_venc : "any",
                 (char *)&p_sys->i_vcodec );
        module_unneed( id->p_decoder, id->p_decoder->p_module );
        id->p_decoder->p_module = 0;
        free( id->p_decoder->p_owner );
        return VLC_EGENERIC;
    }
    /* Close the encoder.
     * We'll open it only when we have the first frame. */
    module_unneed( id->p_encoder, id->p_encoder->p_module );
    if( id->p_encoder->fmt_out.p_extra )
    {
        free( id->p_encoder->fmt_out.p_extra );
        id->p_encoder->fmt_out.p_extra = NULL;
        id->p_encoder->fmt_out.i_extra = 0;
    }
    id->p_encoder->p_module = NULL;
    if( p_sys->i_threads >= 1 )
    {
        int i_priority = p_sys->b_high_priority ? VLC_THREAD_PRIORITY_OUTPUT :
                           VLC_THREAD_PRIORITY_VIDEO;
        p_sys->id_video = id;
        vlc_mutex_init( &p_sys->lock_out );
        vlc_cond_init( &p_sys->cond );
        p_sys->pp_pics = picture_fifo_New();
        if( p_sys->pp_pics == NULL )
        {
            msg_Err( p_stream, "cannot create picture fifo" );
            vlc_mutex_destroy( &p_sys->lock_out );
            vlc_cond_destroy( &p_sys->cond );
            module_unneed( id->p_decoder, id->p_decoder->p_module );
            id->p_decoder->p_module = NULL;
            free( id->p_decoder->p_owner );
            return VLC_ENOMEM;
        }
        p_sys->p_buffers = NULL;
        p_sys->b_abort = false;
        if( vlc_clone( &p_sys->thread, EncoderThread, p_sys, i_priority ) )
        {
            msg_Err( p_stream, "cannot spawn encoder thread" );
            vlc_mutex_destroy( &p_sys->lock_out );
            vlc_cond_destroy( &p_sys->cond );
            picture_fifo_Delete( p_sys->pp_pics );
            module_unneed( id->p_decoder, id->p_decoder->p_module );
            id->p_decoder->p_module = NULL;
            free( id->p_decoder->p_owner );
            return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
