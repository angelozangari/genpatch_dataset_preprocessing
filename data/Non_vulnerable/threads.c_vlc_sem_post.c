}
int vlc_sem_post (vlc_sem_t *sem)
{
    int ret = 0;
    vlc_mutex_lock (&sem->lock);
    if (likely(sem->value != UINT_MAX))
        sem->value++;
    else
        ret = EOVERFLOW;
    vlc_mutex_unlock (&sem->lock);
    vlc_cond_signal (&sem->wait);
    return ret;
}
