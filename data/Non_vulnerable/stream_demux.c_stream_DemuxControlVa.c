}
int stream_DemuxControlVa( stream_t *s, int query, va_list args )
{
    stream_sys_t *sys = s->p_sys;
    switch( query )
    {
        case DEMUX_GET_POSITION:
            vlc_mutex_lock( &sys->lock );
            *va_arg( args, double * ) = sys->stats.position;
            vlc_mutex_unlock( &sys->lock );
            break;
        case DEMUX_GET_LENGTH:
            vlc_mutex_lock( &sys->lock );
            *va_arg( args, int64_t * ) = sys->stats.length;
            vlc_mutex_unlock( &sys->lock );
            break;
        case DEMUX_GET_TIME:
            vlc_mutex_lock( &sys->lock );
            *va_arg( args, int64_t * ) = sys->stats.time;
            vlc_mutex_unlock( &sys->lock );
            break;
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
