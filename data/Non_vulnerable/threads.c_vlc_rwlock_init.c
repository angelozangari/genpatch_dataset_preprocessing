#define WRITER_BIT  LONG_MIN
void vlc_rwlock_init (vlc_rwlock_t *lock)
{
    vlc_mutex_init (&lock->mutex);
    vlc_cond_init (&lock->wait);
    lock->state = 0;
}
