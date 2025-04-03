}
int transcode_spu_new( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    /*
     * Open decoder
     */
    /* Initialization of decoder structures */
    id->p_decoder->pf_decode_sub = NULL;
    id->p_decoder->pf_spu_buffer_new = spu_new_buffer;
    id->p_decoder->pf_spu_buffer_del = spu_del_buffer;
    id->p_decoder->p_owner = (decoder_owner_sys_t *)p_stream;
    /* id->p_decoder->p_cfg = p_sys->p_spu_cfg; */
    id->p_decoder->p_module =
        module_need( id->p_decoder, "decoder", "$codec", false );
    if( !id->p_decoder->p_module )
    {
        msg_Err( p_stream, "cannot find spu decoder" );
        return VLC_EGENERIC;
    }
    if( !p_sys->b_soverlay )
    {
        /* Open encoder */
        /* Initialization of encoder format structures */
        es_format_Init( &id->p_encoder->fmt_in, id->p_decoder->fmt_in.i_cat,
                        id->p_decoder->fmt_in.i_codec );
        id->p_encoder->p_cfg = p_sys->p_spu_cfg;
        id->p_encoder->p_module =
            module_need( id->p_encoder, "encoder", p_sys->psz_senc, true );
        if( !id->p_encoder->p_module )
        {
            module_unneed( id->p_decoder, id->p_decoder->p_module );
            msg_Err( p_stream, "cannot find spu encoder (%s)", p_sys->psz_senc );
            return VLC_EGENERIC;
        }
    }
    if( !p_sys->p_spu )
        p_sys->p_spu = spu_Create( p_stream );
    return VLC_SUCCESS;
}
