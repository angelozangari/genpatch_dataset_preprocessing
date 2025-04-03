}
int transcode_spu_process( sout_stream_t *p_stream,
                                  sout_stream_id_sys_t *id,
                                  block_t *in, block_t **out )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    subpicture_t *p_subpic;
    *out = NULL;
    p_subpic = id->p_decoder->pf_decode_sub( id->p_decoder, &in );
    if( !p_subpic )
    {
        /* We just don't have anything to handle now, go own*/
        return VLC_SUCCESS;
    }
    if( p_sys->b_master_sync && p_sys->i_master_drift )
    {
        p_subpic->i_start -= p_sys->i_master_drift;
        if( p_subpic->i_stop ) p_subpic->i_stop -= p_sys->i_master_drift;
    }
    if( p_sys->b_soverlay )
    {
        spu_PutSubpicture( p_sys->p_spu, p_subpic );
        return VLC_SUCCESS;
    }
    else
    {
        block_t *p_block;
        p_block = id->p_encoder->pf_encode_sub( id->p_encoder, p_subpic );
        spu_del_buffer( id->p_decoder, p_subpic );
        if( p_block )
        {
            block_ChainAppend( out, p_block );
            return VLC_SUCCESS;
        }
    }
    return VLC_EGENERIC;
}
