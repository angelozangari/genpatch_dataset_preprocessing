}
void* update_CheckReal( void *obj )
{
    update_check_thread_t *p_uct = (update_check_thread_t *)obj;
    bool b_ret;
    int canc;
    canc = vlc_savecancel ();
    vlc_mutex_lock( &p_uct->p_update->lock );
    EmptyRelease( p_uct->p_update );
    b_ret = GetUpdateFile( p_uct->p_update );
    vlc_mutex_unlock( &p_uct->p_update->lock );
    if( p_uct->pf_callback )
        (p_uct->pf_callback)( p_uct->p_data, b_ret );
    vlc_restorecancel (canc);
    return NULL;
}
