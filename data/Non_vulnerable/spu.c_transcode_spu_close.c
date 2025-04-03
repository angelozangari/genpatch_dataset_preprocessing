}
void transcode_spu_close( sout_stream_t *p_stream, sout_stream_id_sys_t *id)
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    /* Close decoder */
    if( id->p_decoder->p_module )
        module_unneed( id->p_decoder, id->p_decoder->p_module );
    if( id->p_decoder->p_description )
        vlc_meta_Delete( id->p_decoder->p_description );
    /* Close encoder */
    if( id->p_encoder->p_module )
        module_unneed( id->p_encoder, id->p_encoder->p_module );
    if( p_sys->p_spu )
    {
        spu_Destroy( p_sys->p_spu );
        p_sys->p_spu = NULL;
    }
}
