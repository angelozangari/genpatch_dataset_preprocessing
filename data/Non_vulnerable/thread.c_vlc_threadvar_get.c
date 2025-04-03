}
void *vlc_threadvar_get (vlc_threadvar_t key)
{
    int saved = GetLastError ();
    void *value = TlsGetValue (key->id);
    SetLastError(saved);
    return value;
}
