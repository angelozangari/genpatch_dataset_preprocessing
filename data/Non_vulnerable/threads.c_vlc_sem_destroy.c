}
void vlc_sem_destroy (vlc_sem_t *sem)
{
    vlc_cond_destroy (&sem->wait);
    vlc_mutex_destroy (&sem->lock);
}
