 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    filter_t * p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    vlc_cancel( p_sys->thread );
    vlc_join( p_sys->thread, NULL );
    block_FifoRelease( p_sys->fifo );
    aout_filter_RequestVout( p_filter, p_filter->p_sys->p_vout, NULL );
    /* Free the list */
    for( int i = 0; i < p_sys->i_effect; i++ )
    {
#define p_effect (p_sys->effect[i])
        p_effect->pf_free( p_effect->p_data );
        free( p_effect );
#undef p_effect
    }
    free( p_sys->effect );
    free( p_sys );
}
