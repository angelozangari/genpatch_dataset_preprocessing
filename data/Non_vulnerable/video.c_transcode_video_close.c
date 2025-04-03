}
void transcode_video_close( sout_stream_t *p_stream,
                                   sout_stream_id_sys_t *id )
{
    if( p_stream->p_sys->i_threads >= 1 )
    {
        vlc_mutex_lock( &p_stream->p_sys->lock_out );
        p_stream->p_sys->b_abort = true;
        vlc_cond_signal( &p_stream->p_sys->cond );
        vlc_mutex_unlock( &p_stream->p_sys->lock_out );
        vlc_join( p_stream->p_sys->thread, NULL );
        vlc_mutex_destroy( &p_stream->p_sys->lock_out );
        vlc_cond_destroy( &p_stream->p_sys->cond );
        picture_fifo_Delete( p_stream->p_sys->pp_pics );
        block_ChainRelease( p_stream->p_sys->p_buffers );
        p_stream->p_sys->pp_pics = NULL;
    }
    /* Close decoder */
    if( id->p_decoder->p_module )
        module_unneed( id->p_decoder, id->p_decoder->p_module );
    if( id->p_decoder->p_description )
        vlc_meta_Delete( id->p_decoder->p_description );
    free( id->p_decoder->p_owner );
    /* Close encoder */
    if( id->p_encoder->p_module )
        module_unneed( id->p_encoder, id->p_encoder->p_module );
    /* Close filters */
    if( id->p_f_chain )
        filter_chain_Delete( id->p_f_chain );
    if( id->p_uf_chain )
        filter_chain_Delete( id->p_uf_chain );
}
