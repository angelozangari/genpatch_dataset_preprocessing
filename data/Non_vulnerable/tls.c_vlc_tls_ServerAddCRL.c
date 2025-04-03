 */
int vlc_tls_ServerAddCRL (vlc_tls_creds_t *srv, const char *path)
{
    return srv->add_CRL (srv, path);
}
