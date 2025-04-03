 **/
vlc_tls_creds_t *vlc_tls_ClientCreate (vlc_object_t *obj)
{
    vlc_tls_creds_t *crd = vlc_custom_create (obj, sizeof (*crd),
                                              "tls client");
    if (unlikely(crd == NULL))
        return NULL;
    crd->module = vlc_module_load (crd, "tls client", NULL, false,
                                   tls_client_load, crd);
    if (crd->module == NULL)
    {
        msg_Err (crd, "TLS client plugin not available");
        vlc_object_release (crd);
        return NULL;
    }
    return crd;
}
