}
static int DStreamControl( stream_t *s, int i_query, va_list args )
{
    stream_sys_t *p_sys = s->p_sys;
    uint64_t    *p_i64;
    switch( i_query )
    {
        case STREAM_GET_SIZE:
            p_i64 = va_arg( args, uint64_t * );
            *p_i64 = 0;
            return VLC_SUCCESS;
        case STREAM_CAN_SEEK:
        case STREAM_CAN_FASTSEEK:
        case STREAM_CAN_PAUSE:
        case STREAM_CAN_CONTROL_PACE:
            *va_arg( args, bool * ) = false;
            return VLC_SUCCESS;
        case STREAM_GET_POSITION:
            p_i64 = va_arg( args, uint64_t * );
            *p_i64 = p_sys->i_pos;
            return VLC_SUCCESS;
        case STREAM_SET_POSITION:
        {
            uint64_t i64 = va_arg( args, uint64_t );
            if( i64 < p_sys->i_pos )
                return VLC_EGENERIC;
            uint64_t i_skip = i64 - p_sys->i_pos;
            while( i_skip > 0 )
            {
                int i_read = DStreamRead( s, NULL, __MIN(i_skip, INT_MAX) );
                if( i_read <= 0 )
                    return VLC_EGENERIC;
                i_skip -= i_read;
            }
            return VLC_SUCCESS;
        }
        case STREAM_GET_PTS_DELAY:
            *va_arg( args, int64_t * ) = DEFAULT_PTS_DELAY;
            return VLC_SUCCESS;
        case STREAM_GET_TITLE_INFO:
        case STREAM_GET_TITLE:
        case STREAM_GET_SEEKPOINT:
        case STREAM_GET_META:
        case STREAM_GET_CONTENT_TYPE:
        case STREAM_GET_SIGNAL:
        case STREAM_SET_PAUSE_STATE:
        case STREAM_SET_TITLE:
        case STREAM_SET_SEEKPOINT:
        case STREAM_SET_RECORD_STATE:
        case STREAM_SET_PRIVATE_ID_STATE:
        case STREAM_SET_PRIVATE_ID_CA:
        case STREAM_GET_PRIVATE_ID_STATE:
            return VLC_EGENERIC;
        default:
            msg_Err( s, "invalid DStreamControl query=0x%x", i_query );
            return VLC_EGENERIC;
    }
}
