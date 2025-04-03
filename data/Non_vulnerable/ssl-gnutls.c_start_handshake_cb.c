static gboolean
start_handshake_cb(gpointer data)
{
	PurpleSslConnection *gsc = data;
	PurpleSslGnutlsData *gnutls_data = PURPLE_SSL_GNUTLS_DATA(gsc);
	purple_debug_info("gnutls", "Starting handshake with %s\n", gsc->host);
	gnutls_data->handshake_timer = 0;
	ssl_gnutls_handshake_cb(gsc, gsc->fd, PURPLE_INPUT_READ);
	return FALSE;
}
