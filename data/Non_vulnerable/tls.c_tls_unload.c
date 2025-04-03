}
static void tls_unload(void *func, va_list ap)
{
    void (*deactivate) (vlc_tls_creds_t *) = func;
    vlc_tls_creds_t *crd = va_arg (ap, vlc_tls_creds_t *);
    deactivate (crd);
}
