PurpleSslConnection *
purple_ssl_connect_with_host_fd(PurpleAccount *account, int fd,
                      PurpleSslInputFunction func,
                      PurpleSslErrorFunction error_func,
                      const char *host,
                      void *data)
{
	PurpleSslConnection *gsc;
	PurpleSslOps *ops;
	g_return_val_if_fail(fd != -1,                NULL);
	g_return_val_if_fail(func != NULL,            NULL);
	g_return_val_if_fail(purple_ssl_is_supported(), NULL);
	if (!_ssl_initialized)
	{
		if (!ssl_init())
			return NULL;
	}
	gsc = g_new0(PurpleSslConnection, 1);
	gsc->connect_cb_data = data;
	gsc->connect_cb      = func;
	gsc->error_cb        = error_func;
	gsc->fd              = fd;
	if (host) {
		gsc->host = g_strdup(host);
	}
	/* TODO: Move this elsewhere */
	gsc->verifier = purple_certificate_find_verifier("x509","tls_cached");
	ops = purple_ssl_get_ops();
	ops->connectfunc(gsc);
	return (PurpleSslConnection *)gsc;
}
