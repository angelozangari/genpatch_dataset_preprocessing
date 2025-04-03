void
purple_ssl_uninit(void)
{
	PurpleSslOps *ops;
	if (!_ssl_initialized)
		return;
	ops = purple_ssl_get_ops();
	ops->uninit();
	_ssl_initialized = FALSE;
}
