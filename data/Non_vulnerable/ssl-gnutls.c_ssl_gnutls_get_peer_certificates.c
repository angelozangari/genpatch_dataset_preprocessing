static GList *
ssl_gnutls_get_peer_certificates(PurpleSslConnection * gsc)
{
	PurpleSslGnutlsData *gnutls_data = PURPLE_SSL_GNUTLS_DATA(gsc);
	PurpleCertificate *prvcrt = NULL;
	/* List of Certificate instances to return */
	GList * peer_certs = NULL;
	/* List of raw certificates as given by GnuTLS */
	const gnutls_datum_t *cert_list;
	unsigned int cert_list_size = 0;
	unsigned int i;
	/* This should never, ever happen. */
	g_return_val_if_fail( gnutls_certificate_type_get (gnutls_data->session) == GNUTLS_CRT_X509, NULL);
	/* Get the certificate list from GnuTLS */
	/* TODO: I am _pretty sure_ this doesn't block or do other exciting things */
	cert_list = gnutls_certificate_get_peers(gnutls_data->session,
						 &cert_list_size);
	/* Convert each certificate to a Certificate and append it to the list */
	for (i = 0; i < cert_list_size; i++) {
		PurpleCertificate * newcrt = x509_import_from_datum(cert_list[i],
							      GNUTLS_X509_FMT_DER);
		/* Append is somewhat inefficient on linked lists, but is easy
		   to read. If someone complains, I'll change it.
		   TODO: Is anyone complaining? (Maybe elb?) */
		/* only append if previous cert was actually signed by this one.
		 * Thanks Microsoft. */
		if ((newcrt != NULL) && ((prvcrt == NULL) || x509_certificate_signed_by(prvcrt, newcrt))) {
			peer_certs = g_list_append(peer_certs, newcrt);
			prvcrt = newcrt;
		} else {
			x509_destroy_certificate(newcrt);
			purple_debug_error("gnutls", "Dropping further peer certificates "
			                             "because the chain is broken!\n");
			break;
		}
	}
	/* cert_list doesn't need free()-ing */
	return peer_certs;
}
