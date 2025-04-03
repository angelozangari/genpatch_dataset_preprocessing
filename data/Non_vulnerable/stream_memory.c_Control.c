 ****************************************************************************/
static int Control( stream_t *s, int i_query, va_list args )
{
    stream_sys_t *p_sys = s->p_sys;
    uint64_t   *pi_64, i_64;
    switch( i_query )
    {
        case STREAM_GET_SIZE:
            pi_64 = va_arg( args, uint64_t * );
            *pi_64 = p_sys->i_size;
            break;
        case STREAM_CAN_SEEK:
        case STREAM_CAN_FASTSEEK:
        case STREAM_CAN_PAUSE:
        case STREAM_CAN_CONTROL_PACE:
            *va_arg( args, bool * ) = true;
            break;
        case STREAM_GET_POSITION:
            pi_64 = va_arg( args, uint64_t * );
            *pi_64 = p_sys->i_pos;
            break;
        case STREAM_SET_POSITION:
            i_64 = va_arg( args, uint64_t );
            i_64 = __MIN( i_64, s->p_sys->i_size );
            p_sys->i_pos = i_64;
            break;
        case STREAM_GET_PTS_DELAY:
            *va_arg( args, int64_t * ) = 0;
            break;
        case STREAM_GET_TITLE_INFO:
        case STREAM_GET_TITLE:
        case STREAM_GET_SEEKPOINT:
        case STREAM_GET_META:
        case STREAM_GET_CONTENT_TYPE:
        case STREAM_GET_SIGNAL:
        case STREAM_SET_TITLE:
        case STREAM_SET_SEEKPOINT:
            return VLC_EGENERIC;
        case STREAM_SET_PAUSE_STATE:
            break; /* nothing to do */
        case STREAM_SET_PRIVATE_ID_STATE:
        case STREAM_SET_PRIVATE_ID_CA:
        case STREAM_GET_PRIVATE_ID_STATE:
            msg_Err( s, "Hey, what are you thinking? "
                     "DO NOT USE PRIVATE STREAM CONTROLS!!!" );
            return VLC_EGENERIC;
        default:
            msg_Err( s, "invalid stream_vaControl query=0x%x", i_query );
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
