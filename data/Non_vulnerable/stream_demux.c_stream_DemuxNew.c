static void* DStreamThread ( void * );
stream_t *stream_DemuxNew( demux_t *p_demux, const char *psz_demux, es_out_t *out )
{
    vlc_object_t *p_obj = VLC_OBJECT(p_demux);
    /* We create a stream reader, and launch a thread */
    stream_t     *s;
    stream_sys_t *p_sys;
    s = stream_CommonNew( p_obj );
    if( s == NULL )
        return NULL;
    s->p_input = p_demux->p_input;
    s->psz_path  = strdup(""); /* N/A */
    s->pf_read   = DStreamRead;
    s->pf_peek   = DStreamPeek;
    s->pf_control= DStreamControl;
    s->pf_destroy= DStreamDelete;
    s->p_sys = p_sys = malloc( sizeof( *p_sys) );
    if( !s->psz_path || !s->p_sys )
    {
        stream_CommonDelete( s );
        return NULL;
    }
    p_sys->i_pos = 0;
    p_sys->out = out;
    p_sys->p_block = NULL;
    p_sys->psz_name = strdup( psz_demux );
    p_sys->stats.position = 0.;
    p_sys->stats.length = 0;
    p_sys->stats.time = 0;
    /* decoder fifo */
    if( ( p_sys->p_fifo = block_FifoNew() ) == NULL )
    {
        stream_CommonDelete( s );
        free( p_sys->psz_name );
        free( p_sys );
        return NULL;
    }
    atomic_init( &p_sys->active, true );
    vlc_mutex_init( &p_sys->lock );
    if( vlc_clone( &p_sys->thread, DStreamThread, s, VLC_THREAD_PRIORITY_INPUT ) )
    {
        vlc_mutex_destroy( &p_sys->lock );
        block_FifoRelease( p_sys->p_fifo );
        stream_CommonDelete( s );
        free( p_sys->psz_name );
        free( p_sys );
        return NULL;
    }
    return s;
}
