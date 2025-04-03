}
bool transcode_spu_add( sout_stream_t *p_stream, es_format_t *p_fmt,
                        sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if( p_sys->i_scodec )
    {
        msg_Dbg( p_stream, "creating subtitle transcoding from fcc=`%4.4s' "
                 "to fcc=`%4.4s'", (char*)&p_fmt->i_codec,
                 (char*)&p_sys->i_scodec );
        /* Complete destination format */
        id->p_encoder->fmt_out.i_codec = p_sys->i_scodec;
        /* build decoder -> filter -> encoder */
        if( transcode_spu_new( p_stream, id ) )
        {
            msg_Err( p_stream, "cannot create subtitle chain" );
            return false;
        }
        /* open output stream */
        id->id = sout_StreamIdAdd( p_stream->p_next, &id->p_encoder->fmt_out );
        id->b_transcode = true;
        if( !id->id )
        {
            transcode_spu_close( p_stream, id );
            return false;
        }
    }
    else
    {
        assert( p_sys->b_soverlay );
        msg_Dbg( p_stream, "subtitle (fcc=`%4.4s') overlaying",
                 (char*)&p_fmt->i_codec );
        id->b_transcode = true;
        /* Build decoder -> filter -> overlaying chain */
        if( transcode_spu_new( p_stream, id ) )
        {
            msg_Err( p_stream, "cannot create subtitle chain" );
            return false;
        }
    }
    return true;
}
