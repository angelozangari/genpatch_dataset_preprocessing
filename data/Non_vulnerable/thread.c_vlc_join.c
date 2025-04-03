}
void vlc_join (vlc_thread_t th, void **result)
{
    do
        vlc_testcancel ();
    while (vlc_WaitForSingleObject (th->id, INFINITE) == WAIT_IO_COMPLETION);
    if (result != NULL)
        *result = th->data;
    CloseHandle (th->id);
    free (th);
}
