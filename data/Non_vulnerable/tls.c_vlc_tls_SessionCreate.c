/*** TLS  session ***/
vlc_tls_t *vlc_tls_SessionCreate (vlc_tls_creds_t *crd, int fd,
                                  const char *host)
{
    vlc_tls_t *session = vlc_custom_create (crd, sizeof (*session),
                                            "tls session");
    int val = crd->open (crd, session, fd, host);
    if (val == VLC_SUCCESS)
        return session;
    vlc_object_release (session);
    return NULL;
}
