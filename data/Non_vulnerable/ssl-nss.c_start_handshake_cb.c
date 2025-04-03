static gboolean
start_handshake_cb(gpointer data)
{
	PurpleSslConnection *gsc = data;
	PurpleSslNssData *nss_data = PURPLE_SSL_NSS_DATA(gsc);
	nss_data->handshake_timer = 0;
	ssl_nss_handshake_cb(gsc, gsc->fd, PURPLE_INPUT_READ);
	return FALSE;
}
