PurpleSslConnection *
purple_ssl_connect(PurpleAccount *account, const char *host, int port,
				 PurpleSslInputFunction func, PurpleSslErrorFunction error_func,
				 void *data)
{
	return purple_ssl_connect_with_ssl_cn(account, host, port, func, error_func,
	                                  NULL, data);
}
