}
int vlm_Control( vlm_t *p_vlm, int i_query, ... )
{
    va_list args;
    int     i_result;
    va_start( args, i_query );
    vlc_mutex_lock( &p_vlm->lock );
    i_result = vlm_vaControlInternal( p_vlm, i_query, args );
    vlc_mutex_unlock( &p_vlm->lock );
    va_end( args );
    return i_result;
}
