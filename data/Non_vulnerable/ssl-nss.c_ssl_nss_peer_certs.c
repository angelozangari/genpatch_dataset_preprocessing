static GList *
ssl_nss_peer_certs(PurpleSslConnection *gsc)
{
#if 0
	PurpleSslNssData *nss_data = PURPLE_SSL_NSS_DATA(gsc);
	CERTCertificate *cert;
/*
	GList *chain = NULL;
	void *pinArg;
	SECStatus status;
*/
	/* TODO: this is a blind guess */
	cert = SSL_PeerCertificate(nss_data->fd);
	if (cert)
		CERT_DestroyCertificate(cert);
#endif
	return NULL;
}
