}
void vlc_rwlock_unlock (vlc_rwlock_t *lock)
{
    vlc_mutex_lock (&lock->mutex);
    if (lock->state < 0)
    {   /* Write unlock */
        assert (lock->state == WRITER_BIT);
        /* Let reader and writer compete. OS scheduler decides who wins. */
        lock->state = 0;
        vlc_cond_broadcast (&lock->wait);
    }
    else
    {   /* Read unlock */
        assert (lock->state > 0);
        /* If there are no readers left, wake up one pending writer. */
        if (--lock->state == 0)
            vlc_cond_signal (&lock->wait);
    }
    vlc_mutex_unlock (&lock->mutex);
}
