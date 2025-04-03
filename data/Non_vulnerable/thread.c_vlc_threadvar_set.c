}
int vlc_threadvar_set (vlc_threadvar_t key, void *value)
{
    int saved = GetLastError ();
    int val = TlsSetValue (key->id, value) ? ENOMEM : 0;
    if (val == 0)
        SetLastError(saved);
    return val;
}
