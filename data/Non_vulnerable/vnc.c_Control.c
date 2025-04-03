 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    bool *pb;
    int64_t *pi64;
    double *p_dbl;
    vlc_meta_t *p_meta;
    switch( i_query )
    {
        case DEMUX_CAN_PAUSE:
        case DEMUX_CAN_SEEK:
        case DEMUX_CAN_CONTROL_PACE:
        case DEMUX_CAN_CONTROL_RATE:
        case DEMUX_HAS_UNSUPPORTED_META:
            pb = (bool*)va_arg( args, bool * );
            *pb = false;
            return VLC_SUCCESS;
        case DEMUX_CAN_RECORD:
            pb = (bool*)va_arg( args, bool * );
            *pb = true;
            return VLC_SUCCESS;
        case DEMUX_GET_PTS_DELAY:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            *pi64 = INT64_C(1000)
                  * var_InheritInteger( p_demux, "network-caching" );
            return VLC_SUCCESS;
        case DEMUX_GET_TIME:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            *pi64 = mdate() - p_demux->p_sys->i_starttime;
            return VLC_SUCCESS;
        case DEMUX_GET_LENGTH:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            *pi64 = 0;
            return VLC_SUCCESS;
        case DEMUX_GET_FPS:
            p_dbl = (double*)va_arg( args, double * );
            *p_dbl = p_demux->p_sys->f_fps;
            return VLC_SUCCESS;
        case DEMUX_GET_META:
            p_meta = (vlc_meta_t*)va_arg( args, vlc_meta_t* );
            vlc_meta_Set( p_meta, vlc_meta_Title, p_demux->psz_location );
            return VLC_SUCCESS;
        default:
            return VLC_EGENERIC;
    }
}
