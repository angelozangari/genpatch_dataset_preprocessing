/* Take care of the scaling and chroma conversions. */
static void conversion_video_filter_append( sout_stream_id_sys_t *id )
{
    const es_format_t *p_fmt_out = &id->p_decoder->fmt_out;
    if( id->p_f_chain )
        p_fmt_out = filter_chain_GetFmtOut( id->p_f_chain );
    if( id->p_uf_chain )
        p_fmt_out = filter_chain_GetFmtOut( id->p_uf_chain );
    if( ( p_fmt_out->video.i_chroma != id->p_encoder->fmt_in.video.i_chroma ) ||
        ( p_fmt_out->video.i_width != id->p_encoder->fmt_in.video.i_width ) ||
        ( p_fmt_out->video.i_height != id->p_encoder->fmt_in.video.i_height ) )
    {
        filter_chain_AppendFilter( id->p_uf_chain ? id->p_uf_chain : id->p_f_chain,
                                   NULL, NULL,
                                   p_fmt_out,
                                   &id->p_encoder->fmt_in );
    }
}
