static void
ssl_nss_close(PurpleSslConnection *gsc)
{
	PurpleSslNssData *nss_data = PURPLE_SSL_NSS_DATA(gsc);
	if(!nss_data)
		return;
	if (nss_data->in) {
		PR_Close(nss_data->in);
		gsc->fd = -1;
	} else if (nss_data->fd) {
		PR_Close(nss_data->fd);
		gsc->fd = -1;
	}
	if (nss_data->handshake_handler)
		purple_input_remove(nss_data->handshake_handler);
	if (nss_data->handshake_timer)
		purple_timeout_remove(nss_data->handshake_timer);
	g_free(nss_data);
	gsc->private_data = NULL;
}
