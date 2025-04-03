}
int vlc_tls_SessionHandshake (vlc_tls_t *session, const char *host,
                              const char *service)
{
    return session->handshake (session, host, service);
}
