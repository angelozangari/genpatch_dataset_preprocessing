static GList *
ssl_nss_get_peer_certificates(PRFileDesc *socket, PurpleSslConnection * gsc)
{
	CERTCertificate *curcert;
	CERTCertificate *issuerCert;
	PurpleCertificate * newcrt;
	/* List of Certificate instances to return */
	GList * peer_certs = NULL;
	int count;
	int64 now = PR_Now();
	curcert = SSL_PeerCertificate(socket);
	if (curcert == NULL) {
		purple_debug_error("nss", "could not DupCertificate\n");
		return NULL;
	}
	for (count = 0 ; count < CERT_MAX_CERT_CHAIN ; count++) {
		purple_debug_info("nss", "subject=%s issuer=%s\n", curcert->subjectName,
						  curcert->issuerName  ? curcert->issuerName : "(null)");
		newcrt = x509_import_from_nss(curcert);
		peer_certs = g_list_append(peer_certs, newcrt);
		if (curcert->isRoot) {
			break;
		}
		issuerCert = CERT_FindCertIssuer(curcert, now, certUsageSSLServer);
		if (!issuerCert) {
			purple_debug_error("nss", "partial certificate chain\n");
			break;
		}
		CERT_DestroyCertificate(curcert);
		curcert = issuerCert;
	}
	CERT_DestroyCertificate(curcert);
	return peer_certs;
}
