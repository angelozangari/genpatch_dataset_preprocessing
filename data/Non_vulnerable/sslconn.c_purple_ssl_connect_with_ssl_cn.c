PurpleSslConnection *
purple_ssl_connect_with_ssl_cn(PurpleAccount *account, const char *host, int port,
				 PurpleSslInputFunction func, PurpleSslErrorFunction error_func,
				 const char *ssl_cn, void *data)
{
	PurpleSslConnection *gsc;
	g_return_val_if_fail(host != NULL,            NULL);
	g_return_val_if_fail(port != 0 && port != -1, NULL);
	g_return_val_if_fail(func != NULL,            NULL);
	g_return_val_if_fail(purple_ssl_is_supported(), NULL);
	if (!_ssl_initialized)
	{
		if (!ssl_init())
			return NULL;
	}
	gsc = g_new0(PurpleSslConnection, 1);
	gsc->fd              = -1;
	gsc->host            = ssl_cn ? g_strdup(ssl_cn) : g_strdup(host);
	gsc->port            = port;
	gsc->connect_cb_data = data;
	gsc->connect_cb      = func;
	gsc->error_cb        = error_func;
	/* TODO: Move this elsewhere */
	gsc->verifier = purple_certificate_find_verifier("x509","tls_cached");
	gsc->connect_data = purple_proxy_connect(NULL, account, host, port, purple_ssl_connect_cb, gsc);
	if (gsc->connect_data == NULL)
	{
		g_free(gsc->host);
		g_free(gsc);
		return NULL;
	}
	return (PurpleSslConnection *)gsc;
}
