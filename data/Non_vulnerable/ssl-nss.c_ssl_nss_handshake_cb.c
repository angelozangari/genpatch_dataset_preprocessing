static void
ssl_nss_handshake_cb(gpointer data, int fd, PurpleInputCondition cond)
{
	PurpleSslConnection *gsc = (PurpleSslConnection *)data;
	PurpleSslNssData *nss_data = gsc->private_data;
	/* I don't think this the best way to do this...
	 * It seems to work because it'll eventually use the cached value
	 */
	if(SSL_ForceHandshake(nss_data->in) != SECSuccess) {
		gchar *error_txt;
		set_errno(PR_GetError());
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		error_txt = get_error_text();
		purple_debug_error("nss", "Handshake failed %s (%d)\n", error_txt ? error_txt : "", PR_GetError());
		g_free(error_txt);
		if (gsc->error_cb != NULL)
			gsc->error_cb(gsc, PURPLE_SSL_HANDSHAKE_FAILED, gsc->connect_cb_data);
		purple_ssl_close(gsc);
		return;
	}
	print_security_info(nss_data->in);
	purple_input_remove(nss_data->handshake_handler);
	nss_data->handshake_handler = 0;
	/* If a Verifier was given, hand control over to it */
	if (gsc->verifier) {
		GList *peers;
		/* First, get the peer cert chain */
		peers = ssl_nss_get_peer_certificates(nss_data->in, gsc);
		/* Now kick off the verification process */
		purple_certificate_verify(gsc->verifier,
				gsc->host,
				peers,
				ssl_nss_verified_cb,
				gsc);
		purple_certificate_destroy_list(peers);
	} else {
		/* Otherwise, just call the "connection complete"
		 * callback. The verification was already done with
		 * SSL_AuthCertificate, the default verifier
		 * (SSL_AuthCertificateHook was not called in ssl_nss_connect).
		 */
		gsc->connect_cb(gsc->connect_cb_data, gsc, cond);
	}
}
