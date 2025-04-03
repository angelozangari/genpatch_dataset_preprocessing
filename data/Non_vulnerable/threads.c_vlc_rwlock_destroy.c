}
void vlc_rwlock_destroy (vlc_rwlock_t *lock)
{
    vlc_cond_destroy (&lock->wait);
    vlc_mutex_destroy (&lock->mutex);
}
