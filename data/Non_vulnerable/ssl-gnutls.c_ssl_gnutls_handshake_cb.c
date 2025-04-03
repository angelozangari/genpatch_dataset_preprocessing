}
static void ssl_gnutls_handshake_cb(gpointer data, gint source,
		PurpleInputCondition cond)
{
	PurpleSslConnection *gsc = data;
	PurpleSslGnutlsData *gnutls_data = PURPLE_SSL_GNUTLS_DATA(gsc);
	ssize_t ret;
	/*purple_debug_info("gnutls", "Handshaking with %s\n", gsc->host);*/
	ret = gnutls_handshake(gnutls_data->session);
	if(ret == GNUTLS_E_AGAIN || ret == GNUTLS_E_INTERRUPTED)
		return;
	purple_input_remove(gnutls_data->handshake_handler);
	gnutls_data->handshake_handler = 0;
	if(ret != 0) {
		purple_debug_error("gnutls", "Handshake failed. Error %s\n",
			gnutls_strerror(ret));
		if(gsc->error_cb != NULL)
			gsc->error_cb(gsc, PURPLE_SSL_HANDSHAKE_FAILED,
				gsc->connect_cb_data);
		purple_ssl_close(gsc);
	} else {
		/* Now we are cooking with gas! */
		PurpleSslOps *ops = purple_ssl_get_ops();
		GList * peers = ops->get_peer_certificates(gsc);
		PurpleCertificateScheme *x509 =
			purple_certificate_find_scheme("x509");
		GList * l;
		/* TODO: Remove all this debugging babble */
		purple_debug_info("gnutls", "Handshake complete\n");
		for (l=peers; l; l = l->next) {
			PurpleCertificate *crt = l->data;
			GByteArray *z =
				x509->get_fingerprint_sha1(crt);
			gchar * fpr =
				purple_base16_encode_chunked(z->data,
							     z->len);
			purple_debug_info("gnutls/x509",
					  "Key print: %s\n",
					  fpr);
			/* Kill the cert! */
			x509->destroy_certificate(crt);
			g_free(fpr);
			g_byte_array_free(z, TRUE);
		}
		g_list_free(peers);
		{
			const gnutls_datum_t *cert_list;
			unsigned int cert_list_size = 0;
			gnutls_session_t session=gnutls_data->session;
			guint i;
			cert_list =
				gnutls_certificate_get_peers(session, &cert_list_size);
			purple_debug_info("gnutls",
					    "Peer provided %d certs\n",
					    cert_list_size);
			for (i=0; i<cert_list_size; i++)
			{
				gchar fpr_bin[256];
				gsize fpr_bin_sz = sizeof(fpr_bin);
				gchar * fpr_asc = NULL;
				gchar tbuf[256];
				gsize tsz=sizeof(tbuf);
				gchar * tasc = NULL;
				gnutls_x509_crt_t cert;
				gnutls_x509_crt_init(&cert);
				gnutls_x509_crt_import (cert, &cert_list[i],
						GNUTLS_X509_FMT_DER);
				gnutls_x509_crt_get_fingerprint(cert, GNUTLS_DIG_SHA,
						fpr_bin, &fpr_bin_sz);
				fpr_asc =
						purple_base16_encode_chunked((const guchar *)fpr_bin, fpr_bin_sz);
				purple_debug_info("gnutls",
						"Lvl %d SHA1 fingerprint: %s\n",
						i, fpr_asc);
				tsz=sizeof(tbuf);
				gnutls_x509_crt_get_serial(cert,tbuf,&tsz);
				tasc=purple_base16_encode_chunked((const guchar *)tbuf, tsz);
				purple_debug_info("gnutls",
						"Serial: %s\n",
						tasc);
				g_free(tasc);
				tsz=sizeof(tbuf);
				gnutls_x509_crt_get_dn (cert, tbuf, &tsz);
				purple_debug_info("gnutls",
						"Cert DN: %s\n",
						tbuf);
				tsz=sizeof(tbuf);
				gnutls_x509_crt_get_issuer_dn (cert, tbuf, &tsz);
				purple_debug_info("gnutls",
						"Cert Issuer DN: %s\n",
						tbuf);
				g_free(fpr_asc);
				fpr_asc = NULL;
				gnutls_x509_crt_deinit(cert);
			}
		}
		/* TODO: The following logic should really be in libpurple */
		/* If a Verifier was given, hand control over to it */
		if (gsc->verifier) {
			GList *peers;
			/* First, get the peer cert chain */
			peers = purple_ssl_get_peer_certificates(gsc);
			/* Now kick off the verification process */
			purple_certificate_verify(gsc->verifier,
						  gsc->host,
						  peers,
						  ssl_gnutls_verified_cb,
						  gsc);
			purple_certificate_destroy_list(peers);
		} else {
			/* Otherwise, just call the "connection complete"
			   callback */
			gsc->connect_cb(gsc->connect_cb_data, gsc, cond);
		}
	}
}
