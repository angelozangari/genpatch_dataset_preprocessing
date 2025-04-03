 **/
vlc_tls_t *vlc_tls_ClientSessionCreate (vlc_tls_creds_t *crd, int fd,
                                        const char *host, const char *service)
{
    vlc_tls_t *session = vlc_tls_SessionCreate (crd, fd, host);
    if (session == NULL)
        return NULL;
    mtime_t deadline = mdate ();
    deadline += var_InheritInteger (crd, "ipv4-timeout") * 1000;
    struct pollfd ufd[1];
    ufd[0].fd = fd;
    int val;
    while ((val = vlc_tls_SessionHandshake (session, host, service)) > 0)
    {
        mtime_t now = mdate ();
        if (now > deadline)
           now = deadline;
        assert (val <= 2);
        ufd[0] .events = (val == 1) ? POLLIN : POLLOUT;
        if (poll (ufd, 1, (deadline - now) / 1000) == 0)
        {
            msg_Err (session, "TLS client session handshake timeout");
            val = -1;
            break;
        }
    }
    if (val != 0)
    {
        msg_Err (session, "TLS client session handshake error");
        vlc_tls_SessionDelete (session);
        session = NULL;
    }
    return session;
}
