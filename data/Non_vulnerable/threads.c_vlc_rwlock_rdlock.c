}
void vlc_rwlock_rdlock (vlc_rwlock_t *lock)
{
    vlc_mutex_lock (&lock->mutex);
    /* Recursive read-locking is allowed.
     * Ensure that there is no active writer. */
    while (lock->state < 0)
    {
        assert (lock->state == WRITER_BIT);
        mutex_cleanup_push (&lock->mutex);
        vlc_cond_wait (&lock->wait, &lock->mutex);
        vlc_cleanup_pop ();
    }
    if (unlikely(lock->state >= READER_MASK))
        abort (); /* An overflow is certainly a recursion bug. */
    lock->state++;
    vlc_mutex_unlock (&lock->mutex);
}
