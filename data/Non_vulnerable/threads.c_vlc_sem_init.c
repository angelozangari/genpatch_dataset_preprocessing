#include <errno.h>
void vlc_sem_init (vlc_sem_t *sem, unsigned value)
{
    vlc_mutex_init (&sem->lock);
    vlc_cond_init (&sem->wait);
    sem->value = value;
}
