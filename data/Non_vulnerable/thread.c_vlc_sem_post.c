}
int vlc_sem_post (vlc_sem_t *sem)
{
    ReleaseSemaphore (*sem, 1, NULL);
    return 0; /* FIXME */
}
