}
static void* DStreamThread( void *obj )
{
    stream_t *s = (stream_t *)obj;
    stream_sys_t *p_sys = s->p_sys;
    demux_t *p_demux;
    /* Create the demuxer */
    p_demux = demux_New( s, s->p_input, "", p_sys->psz_name, "", s, p_sys->out,
                         false );
    if( p_demux == NULL )
        return NULL;
    /* stream_Demux cannot apply DVB filters.
     * Get all programs and let the E/S output sort them out. */
    demux_Control( p_demux, DEMUX_SET_GROUP, -1, NULL );
    /* Main loop */
    mtime_t next_update = 0;
    while( atomic_load( &p_sys->active ) )
    {
        if( p_demux->info.i_update || mdate() >= next_update )
        {
            double newpos;
            int64_t newlen, newtime;
            if( demux_Control( p_demux, DEMUX_GET_POSITION, &newpos ) )
                newpos = 0.;
            if( demux_Control( p_demux, DEMUX_GET_LENGTH, &newlen ) )
                newlen = 0;
            if( demux_Control( p_demux, DEMUX_GET_TIME, &newtime ) )
                newtime = 0;
            vlc_mutex_lock( &p_sys->lock );
            p_sys->stats.position = newpos;
            p_sys->stats.length = newlen;
            p_sys->stats.time = newtime;
            vlc_mutex_unlock( &p_sys->lock );
            p_demux->info.i_update = 0;
            next_update = mdate() + (CLOCK_FREQ / 4);
        }
        if( demux_Demux( p_demux ) <= 0 )
            break;
    }
    /* Explicit kludge: the stream is destroyed by the owner of the
     * streamDemux, not here. */
    p_demux->s = NULL;
    demux_Delete( p_demux );
    return NULL;
}
