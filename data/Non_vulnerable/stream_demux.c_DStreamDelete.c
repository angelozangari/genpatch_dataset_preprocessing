}
static void DStreamDelete( stream_t *s )
{
    stream_sys_t *p_sys = s->p_sys;
    block_t *p_empty;
    atomic_store( &p_sys->active, false );
    p_empty = block_Alloc( 0 );
    block_FifoPut( p_sys->p_fifo, p_empty );
    vlc_join( p_sys->thread, NULL );
    vlc_mutex_destroy( &p_sys->lock );
    if( p_sys->p_block )
        block_Release( p_sys->p_block );
    block_FifoRelease( p_sys->p_fifo );
    free( p_sys->psz_name );
    free( p_sys );
    stream_CommonDelete( s );
}
