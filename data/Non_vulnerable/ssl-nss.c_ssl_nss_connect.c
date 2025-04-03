static void
ssl_nss_connect(PurpleSslConnection *gsc)
{
	PurpleSslNssData *nss_data = g_new0(PurpleSslNssData, 1);
	PRSocketOptionData socket_opt;
	gsc->private_data = nss_data;
	nss_data->fd = PR_ImportTCPSocket(gsc->fd);
	if (nss_data->fd == NULL)
	{
		purple_debug_error("nss", "nss_data->fd == NULL!\n");
		if (gsc->error_cb != NULL)
			gsc->error_cb(gsc, PURPLE_SSL_CONNECT_FAILED, gsc->connect_cb_data);
		purple_ssl_close((PurpleSslConnection *)gsc);
		return;
	}
	socket_opt.option = PR_SockOpt_Nonblocking;
	socket_opt.value.non_blocking = PR_TRUE;
	if (PR_SetSocketOption(nss_data->fd, &socket_opt) != PR_SUCCESS) {
		gchar *error_txt = get_error_text();
		purple_debug_warning("nss", "unable to set socket into non-blocking mode: %s (%d)\n", error_txt ? error_txt : "", PR_GetError());
		g_free(error_txt);
	}
	nss_data->in = SSL_ImportFD(NULL, nss_data->fd);
	if (nss_data->in == NULL)
	{
		purple_debug_error("nss", "nss_data->in == NUL!\n");
		if (gsc->error_cb != NULL)
			gsc->error_cb(gsc, PURPLE_SSL_CONNECT_FAILED, gsc->connect_cb_data);
		purple_ssl_close((PurpleSslConnection *)gsc);
		return;
	}
	SSL_OptionSet(nss_data->in, SSL_SECURITY,            PR_TRUE);
	SSL_OptionSet(nss_data->in, SSL_HANDSHAKE_AS_CLIENT, PR_TRUE);
	/* If we have our internal verifier set up, use it. Otherwise,
	 * use default. */
	if (gsc->verifier != NULL)
		SSL_AuthCertificateHook(nss_data->in, ssl_auth_cert, NULL);
	if(gsc->host)
		SSL_SetURL(nss_data->in, gsc->host);
#if 0
	/* This seems like it'd the be the correct way to implement the
	nonblocking stuff, but it doesn't seem to work */
	SSL_HandshakeCallback(nss_data->in,
		(SSLHandshakeCallback) ssl_nss_handshake_cb, gsc);
#endif
	SSL_ResetHandshake(nss_data->in, PR_FALSE);
	nss_data->handshake_handler = purple_input_add(gsc->fd,
		PURPLE_INPUT_READ, ssl_nss_handshake_cb, gsc);
	nss_data->handshake_timer = purple_timeout_add(0, start_handshake_cb, gsc);
}
