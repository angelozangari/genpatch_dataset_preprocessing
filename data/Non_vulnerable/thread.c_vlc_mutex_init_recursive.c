}
void vlc_mutex_init_recursive( vlc_mutex_t *p_mutex )
{
    InitializeCriticalSection( &p_mutex->mutex );
    p_mutex->dynamic = true;
}
