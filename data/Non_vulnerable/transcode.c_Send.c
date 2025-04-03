}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    block_t *p_out = NULL;
    if( !id->b_transcode )
    {
        if( id->id )
            return sout_StreamIdSend( p_stream->p_next, id->id, p_buffer );
        block_Release( p_buffer );
        return VLC_EGENERIC;
    }
    switch( id->p_decoder->fmt_in.i_cat )
    {
    case AUDIO_ES:
        if( transcode_audio_process( p_stream, id, p_buffer, &p_out )
            != VLC_SUCCESS )
        {
            return VLC_EGENERIC;
        }
        break;
    case VIDEO_ES:
        if( transcode_video_process( p_stream, id, p_buffer, &p_out )
            != VLC_SUCCESS )
        {
            return VLC_EGENERIC;
        }
        break;
    case SPU_ES:
        /* Transcode OSD menu pictures. */
        if( p_sys->b_osd )
        {
            if( transcode_osd_process( p_stream, id, p_buffer, &p_out ) !=
                VLC_SUCCESS )
            {
                return VLC_EGENERIC;
            }
        }
        else if ( transcode_spu_process( p_stream, id, p_buffer, &p_out ) !=
            VLC_SUCCESS )
        {
            return VLC_EGENERIC;
        }
        break;
    default:
        p_out = NULL;
        block_Release( p_buffer );
        break;
    }
    if( p_out )
        return sout_StreamIdSend( p_stream->p_next, id->id, p_out );
    return VLC_SUCCESS;
}
