static void
ssl_gnutls_close(PurpleSslConnection *gsc)
{
	PurpleSslGnutlsData *gnutls_data = PURPLE_SSL_GNUTLS_DATA(gsc);
	if(!gnutls_data)
		return;
	if(gnutls_data->handshake_handler)
		purple_input_remove(gnutls_data->handshake_handler);
	if (gnutls_data->handshake_timer)
		purple_timeout_remove(gnutls_data->handshake_timer);
	gnutls_bye(gnutls_data->session, GNUTLS_SHUT_RDWR);
	gnutls_deinit(gnutls_data->session);
	g_free(gnutls_data);
	gsc->private_data = NULL;
}
