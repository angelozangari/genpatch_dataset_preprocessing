}
static int transcode_video_encoder_open( sout_stream_t *p_stream,
                                         sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    msg_Dbg( p_stream, "destination (after video filters) %ix%i",
             id->p_encoder->fmt_in.video.i_width,
             id->p_encoder->fmt_in.video.i_height );
    id->p_encoder->p_module =
        module_need( id->p_encoder, "encoder", p_sys->psz_venc, true );
    if( !id->p_encoder->p_module )
    {
        msg_Err( p_stream, "cannot find video encoder (module:%s fourcc:%4.4s)",
                 p_sys->psz_venc ? p_sys->psz_venc : "any",
                 (char *)&p_sys->i_vcodec );
        return VLC_EGENERIC;
    }
    id->p_encoder->fmt_in.video.i_chroma = id->p_encoder->fmt_in.i_codec;
    /*  */
    id->p_encoder->fmt_out.i_codec =
        vlc_fourcc_GetCodec( VIDEO_ES, id->p_encoder->fmt_out.i_codec );
    id->id = sout_StreamIdAdd( p_stream->p_next, &id->p_encoder->fmt_out );
    if( !id->id )
    {
        msg_Err( p_stream, "cannot add this stream" );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
