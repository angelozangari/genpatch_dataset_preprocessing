static void
purple_ssl_connect_cb(gpointer data, gint source, const gchar *error_message)
{
	PurpleSslConnection *gsc;
	PurpleSslOps *ops;
	gsc = data;
	gsc->connect_data = NULL;
	if (source < 0)
	{
		if (gsc->error_cb != NULL)
			gsc->error_cb(gsc, PURPLE_SSL_CONNECT_FAILED, gsc->connect_cb_data);
		purple_ssl_close(gsc);
		return;
	}
	gsc->fd = source;
	ops = purple_ssl_get_ops();
	ops->connectfunc(gsc);
}
