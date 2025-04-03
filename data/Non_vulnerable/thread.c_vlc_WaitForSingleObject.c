}
static DWORD vlc_WaitForSingleObject (HANDLE handle, DWORD delay)
{
    return vlc_WaitForMultipleObjects (1, &handle, delay);
}
