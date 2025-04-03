}
void vlc_sem_destroy (vlc_sem_t *sem)
{
    CloseHandle (*sem);
}
