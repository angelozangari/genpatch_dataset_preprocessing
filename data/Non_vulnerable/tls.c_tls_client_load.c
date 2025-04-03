}
static int tls_client_load(void *func, va_list ap)
{
    int (*activate) (vlc_tls_creds_t *) = func;
    vlc_tls_creds_t *crd = va_arg (ap, vlc_tls_creds_t *);
    return activate (crd);
}
