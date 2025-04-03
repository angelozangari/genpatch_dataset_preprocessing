}
static void transcode_video_filter_init( sout_stream_t *p_stream,
                                         sout_stream_id_sys_t *id )
{
    es_format_t *p_fmt_out = &id->p_decoder->fmt_out;
    id->p_encoder->fmt_in.video.i_chroma = id->p_encoder->fmt_in.i_codec;
    id->p_f_chain = filter_chain_New( p_stream, "video filter2",
                                      false,
                                      transcode_video_filter_allocation_init,
                                      transcode_video_filter_allocation_clear,
                                      p_stream->p_sys );
    filter_chain_Reset( id->p_f_chain, p_fmt_out, p_fmt_out );
    /* Deinterlace */
    if( p_stream->p_sys->b_deinterlace )
    {
        filter_chain_AppendFilter( id->p_f_chain,
                                   p_stream->p_sys->psz_deinterlace,
                                   p_stream->p_sys->p_deinterlace_cfg,
                                   &id->p_decoder->fmt_out,
                                   &id->p_decoder->fmt_out );
        p_fmt_out = filter_chain_GetFmtOut( id->p_f_chain );
    }
    /* Check that we have visible_width/height*/
    if( !p_fmt_out->video.i_visible_height )
        p_fmt_out->video.i_visible_height = p_fmt_out->video.i_height;
    if( !p_fmt_out->video.i_visible_width )
        p_fmt_out->video.i_visible_width = p_fmt_out->video.i_width;
    if( p_stream->p_sys->psz_vf2 )
    {
        id->p_uf_chain = filter_chain_New( p_stream, "video filter2",
                                          true,
                           transcode_video_filter_allocation_init,
                           transcode_video_filter_allocation_clear,
                           p_stream->p_sys );
        filter_chain_Reset( id->p_uf_chain, p_fmt_out,
                            &id->p_encoder->fmt_in );
        if( p_fmt_out->video.i_chroma != id->p_encoder->fmt_in.video.i_chroma )
        {
            filter_chain_AppendFilter( id->p_uf_chain,
                                   NULL, NULL,
                                   p_fmt_out,
                                   &id->p_encoder->fmt_in );
        }
        filter_chain_AppendFromString( id->p_uf_chain, p_stream->p_sys->psz_vf2 );
        p_fmt_out = filter_chain_GetFmtOut( id->p_uf_chain );
        es_format_Copy( &id->p_encoder->fmt_in, p_fmt_out );
        id->p_encoder->fmt_out.video.i_width =
            id->p_encoder->fmt_in.video.i_width;
        id->p_encoder->fmt_out.video.i_height =
            id->p_encoder->fmt_in.video.i_height;
        id->p_encoder->fmt_out.video.i_sar_num =
            id->p_encoder->fmt_in.video.i_sar_num;
        id->p_encoder->fmt_out.video.i_sar_den =
            id->p_encoder->fmt_in.video.i_sar_den;
    }
}
