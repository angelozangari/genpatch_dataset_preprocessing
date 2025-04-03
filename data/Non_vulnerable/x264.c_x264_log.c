 ****************************************************************************/
static void x264_log( void *data, int i_level, const char *psz, va_list args)
{
    encoder_t *p_enc = (encoder_t *)data;
    switch( i_level )
    {
        case X264_LOG_ERROR:
            i_level = VLC_MSG_ERR;
            break;
        case X264_LOG_WARNING:
            i_level = VLC_MSG_WARN;
            break;
        case X264_LOG_INFO:
            i_level = VLC_MSG_INFO;
            break;
        case X264_LOG_DEBUG:
        default:
            i_level = VLC_MSG_DBG;
    }
    msg_GenericVa( p_enc, i_level, MODULE_STRING, psz, args );
};
