 */
void vlc_tls_Delete (vlc_tls_creds_t *crd)
{
    if (crd == NULL)
        return;
    vlc_module_unload (crd->module, tls_unload, crd);
    vlc_object_release (crd);
}
