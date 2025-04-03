PurpleSslConnection *
purple_ssl_connect_fd(PurpleAccount *account, int fd,
					PurpleSslInputFunction func,
					PurpleSslErrorFunction error_func,
                    void *data)
{
	return purple_ssl_connect_with_host_fd(account, fd, func, error_func, NULL, data);
}
