}
bool transcode_video_add( sout_stream_t *p_stream, es_format_t *p_fmt,
                                sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    msg_Dbg( p_stream,
             "creating video transcoding from fcc=`%4.4s' to fcc=`%4.4s'",
             (char*)&p_fmt->i_codec, (char*)&p_sys->i_vcodec );
    /* Complete destination format */
    id->p_encoder->fmt_out.i_codec = p_sys->i_vcodec;
    id->p_encoder->fmt_out.video.i_visible_width  = p_sys->i_width & ~1;
    id->p_encoder->fmt_out.video.i_visible_height = p_sys->i_height & ~1;
    id->p_encoder->fmt_out.i_bitrate = p_sys->i_vbitrate;
    /* Build decoder -> filter -> encoder chain */
    if( transcode_video_new( p_stream, id ) )
    {
        msg_Err( p_stream, "cannot create video chain" );
        return false;
    }
    /* Stream will be added later on because we don't know
     * all the characteristics of the decoded stream yet */
    id->b_transcode = true;
    if( p_sys->fps_num )
    {
        id->p_encoder->fmt_out.video.i_frame_rate = (p_sys->fps_num );
        id->p_encoder->fmt_out.video.i_frame_rate_base = (p_sys->fps_den ? p_sys->fps_den : 1);
    }
    return true;
}
