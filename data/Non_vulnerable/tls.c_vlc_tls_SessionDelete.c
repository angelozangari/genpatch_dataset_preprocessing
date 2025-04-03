}
void vlc_tls_SessionDelete (vlc_tls_t *session)
{
    vlc_tls_creds_t *crd = (vlc_tls_creds_t *)(session->p_parent);
    crd->close (crd, session);
    vlc_object_release (session);
}
