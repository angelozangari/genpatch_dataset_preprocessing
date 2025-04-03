}
void vlc_mutex_destroy (vlc_mutex_t *p_mutex)
{
    assert (p_mutex->dynamic);
    DeleteCriticalSection (&p_mutex->mutex);
}
