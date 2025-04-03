vlc_tls_creds_t *
vlc_tls_ServerCreate (vlc_object_t *obj, const char *cert_path,
                      const char *key_path)
{
    vlc_tls_creds_t *srv = vlc_custom_create (obj, sizeof (*srv),
                                              "tls server");
    if (unlikely(srv == NULL))
        return NULL;
    if (key_path == NULL)
        key_path = cert_path;
    srv->module = vlc_module_load (srv, "tls server", NULL, false,
                                   tls_server_load, srv, cert_path, key_path);
    if (srv->module == NULL)
    {
        msg_Err (srv, "TLS server plugin not available");
        vlc_object_release (srv);
        return NULL;
    }
    return srv;
}
