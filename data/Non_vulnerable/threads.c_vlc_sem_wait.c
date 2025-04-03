}
void vlc_sem_wait (vlc_sem_t *sem)
{
    vlc_mutex_lock (&sem->lock);
    mutex_cleanup_push (&sem->lock);
    while (!sem->value)
        vlc_cond_wait (&sem->wait, &sem->lock);
    sem->value--;
    vlc_cleanup_run ();
}
