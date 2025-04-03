}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if( id->b_transcode )
    {
        switch( id->p_decoder->fmt_in.i_cat )
        {
        case AUDIO_ES:
            Send( p_stream, id, NULL );
            transcode_audio_close( id );
            break;
        case VIDEO_ES:
            Send( p_stream, id, NULL );
            transcode_video_close( p_stream, id );
            break;
        case SPU_ES:
            if( p_sys->b_osd )
                transcode_osd_close( p_stream, id );
            else
                transcode_spu_close( p_stream, id );
            break;
        }
    }
    if( id->id ) sout_StreamIdDel( p_stream->p_next, id->id );
    if( id->p_decoder )
    {
        vlc_object_release( id->p_decoder );
        id->p_decoder = NULL;
    }
    if( id->p_encoder )
    {
        es_format_Clean( &id->p_encoder->fmt_out );
        vlc_object_release( id->p_encoder );
        id->p_encoder = NULL;
    }
    free( id );
    return VLC_SUCCESS;
}
