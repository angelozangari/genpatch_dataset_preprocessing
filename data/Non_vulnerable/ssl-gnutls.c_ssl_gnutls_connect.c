static void
ssl_gnutls_connect(PurpleSslConnection *gsc)
{
	PurpleSslGnutlsData *gnutls_data;
	gnutls_data = g_new0(PurpleSslGnutlsData, 1);
	gsc->private_data = gnutls_data;
	gnutls_init(&gnutls_data->session, GNUTLS_CLIENT);
#ifdef HAVE_GNUTLS_PRIORITY_FUNCS
	{
		const char *prio_str = NULL;
		gboolean set = FALSE;
		/* Let's see if someone has specified a specific priority */
		if (gsc->host && host_priorities)
			prio_str = g_hash_table_lookup(host_priorities, gsc->host);
		if (prio_str)
			set = (GNUTLS_E_SUCCESS ==
					gnutls_priority_set_direct(gnutls_data->session, prio_str,
				                               NULL));
		if (!set)
			gnutls_priority_set(gnutls_data->session, default_priority);
	}
#else
	gnutls_set_default_priority(gnutls_data->session);
#endif
	gnutls_credentials_set(gnutls_data->session, GNUTLS_CRD_CERTIFICATE,
		xcred);
	gnutls_transport_set_ptr(gnutls_data->session, GINT_TO_POINTER(gsc->fd));
	gnutls_data->handshake_handler = purple_input_add(gsc->fd,
		PURPLE_INPUT_READ, ssl_gnutls_handshake_cb, gsc);
	/* Orborde asks: Why are we configuring a callback, then
	   (almost) immediately calling it?
	   Answer: gnutls_handshake (up in handshake_cb) needs to be called
	   once in order to get the ball rolling on the SSL connection.
	   Once it has done so, only then will the server reply, triggering
	   the callback.
	   Since the logic driving gnutls_handshake is the same with the first
	   and subsequent calls, we'll just fire the callback immediately to
	   accomplish this.
	*/
	gnutls_data->handshake_timer = purple_timeout_add(0, start_handshake_cb,
	                                                  gsc);
}
