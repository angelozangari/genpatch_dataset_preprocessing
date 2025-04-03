 *****************************************************************************/
static int Control( access_t *p_access, int i_query, va_list args )
{
    access_sys_t *p_sys = p_access->p_sys;
    input_title_t ***ppp_title;
    int i;
    int64_t *pi64;
    vlc_meta_t *p_meta;
    switch( i_query )
    {
        case ACCESS_CAN_SEEK:
        case ACCESS_CAN_FASTSEEK:
        case ACCESS_CAN_PAUSE:
        case ACCESS_CAN_CONTROL_PACE:
            *va_arg( args, bool* ) = true;
            break;
        case ACCESS_GET_SIZE:
            *va_arg( args, uint64_t* ) = p_sys->size;
            break;
        case ACCESS_GET_PTS_DELAY:
            pi64 = va_arg( args, int64_t * );
            *pi64 = INT64_C(1000)
                  * var_InheritInteger( p_access, "file-caching" );
            break;
        case ACCESS_SET_PAUSE_STATE:
            /* nothing to do */
            break;
        case ACCESS_GET_TITLE_INFO:
            /* return a copy of our seek points */
            if( !p_sys->p_marks )
                return VLC_EGENERIC;
            ppp_title = va_arg( args, input_title_t*** );
            *va_arg( args, int* ) = 1;
            *ppp_title = malloc( sizeof( **ppp_title ) );
            if( !*ppp_title )
                return VLC_ENOMEM;
            **ppp_title = vlc_input_title_Duplicate( p_sys->p_marks );
            break;
        case ACCESS_GET_TITLE:
            *va_arg( args, unsigned * ) = 0;
            break;
        case ACCESS_GET_SEEKPOINT:
            *va_arg( args, unsigned * ) = p_sys->cur_seekpoint;
            break;
        case ACCESS_SET_TITLE:
            /* ignore - only one title */
            break;
        case ACCESS_SET_SEEKPOINT:
            i = va_arg( args, int );
            return Seek( p_access, p_sys->p_marks->seekpoint[i]->i_byte_offset );
        case ACCESS_GET_META:
            if( !p_sys->p_meta )
                return VLC_EGENERIC;
            p_meta = va_arg( args, vlc_meta_t* );
            vlc_meta_Merge( p_meta, p_sys->p_meta );
            break;
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
