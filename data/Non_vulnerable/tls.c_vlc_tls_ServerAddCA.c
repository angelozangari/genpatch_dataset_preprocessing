 */
int vlc_tls_ServerAddCA (vlc_tls_creds_t *srv, const char *path)
{
    return srv->add_CA (srv, path);
}
