}
static rfbCredential* getCredentialHandler( rfbClient *p_client, int i_credentialType )
{
    demux_t *p_demux = (demux_t *) rfbClientGetClientData( p_client, DemuxThread );
    rfbCredential *credential = calloc( 1, sizeof(rfbCredential) );
    if ( !credential ) return NULL;
    switch( i_credentialType )
    {
        case rfbCredentialTypeX509:
            /* X509None, X509Vnc, X509Plain */
            credential->x509Credential.x509CACertFile =
                    var_InheritString( p_demux, CFG_PREFIX "x509-ca" );
            credential->x509Credential.x509CACrlFile =
                    var_InheritString( p_demux, CFG_PREFIX "x509-crl" );
            /* client auth by certificate */
            credential->x509Credential.x509ClientCertFile =
                    var_InheritString( p_demux, CFG_PREFIX "x509-client-cert" );
            credential->x509Credential.x509ClientKeyFile =
                    var_InheritString( p_demux, CFG_PREFIX "x509-client-key" );
            break;
        case rfbCredentialTypeUser:
            credential->userCredential.username =
                    var_InheritString( p_demux, CFG_PREFIX "user" );
            credential->userCredential.password =
                    var_InheritString( p_demux, CFG_PREFIX "password" );
            break;
        default:
            free( credential );
            return NULL; /* Unsupported Auth */
    }
    /* freed by libvnc */
    return credential;
}
