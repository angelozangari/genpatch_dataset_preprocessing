}
void vlc_sem_wait (vlc_sem_t *sem)
{
    DWORD result;
    do
    {
        vlc_testcancel ();
        result = vlc_WaitForSingleObject (*sem, INFINITE);
    }
    while (result == WAIT_IO_COMPLETION);
}
