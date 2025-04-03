void
purple_ssl_close(PurpleSslConnection *gsc)
{
	PurpleSslOps *ops;
	g_return_if_fail(gsc != NULL);
	purple_request_close_with_handle(gsc);
	purple_notify_close_with_handle(gsc);
	ops = purple_ssl_get_ops();
	(ops->close)(gsc);
	if (gsc->connect_data != NULL)
		purple_proxy_connect_cancel(gsc->connect_data);
	if (gsc->inpa > 0)
		purple_input_remove(gsc->inpa);
	if (gsc->fd >= 0)
		close(gsc->fd);
	g_free(gsc->host);
	g_free(gsc);
}
