}
void vlc_rwlock_wrlock (vlc_rwlock_t *lock)
{
    vlc_mutex_lock (&lock->mutex);
    /* Wait until nobody owns the lock in any way. */
    while (lock->state != 0)
    {
        mutex_cleanup_push (&lock->mutex);
        vlc_cond_wait (&lock->wait, &lock->mutex);
        vlc_cleanup_pop ();
    }
    lock->state = WRITER_BIT;
    vlc_mutex_unlock (&lock->mutex);
}
