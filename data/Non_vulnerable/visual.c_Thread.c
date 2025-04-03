}
static void *Thread( void *data )
{
    filter_t *p_filter = data;
    filter_sys_t *sys = p_filter->p_sys;
    for (;;)
    {
        block_t *block = block_FifoGet( sys->fifo );
        int canc = vlc_savecancel( );
        block_Release( DoRealWork( p_filter, block ) );
        vlc_restorecancel( canc );
    }
    assert(0);
}
