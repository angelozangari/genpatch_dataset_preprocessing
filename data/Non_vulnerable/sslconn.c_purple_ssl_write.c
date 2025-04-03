size_t
purple_ssl_write(PurpleSslConnection *gsc, const void *data, size_t len)
{
	PurpleSslOps *ops;
	g_return_val_if_fail(gsc  != NULL, 0);
	g_return_val_if_fail(data != NULL, 0);
	g_return_val_if_fail(len  >  0,    0);
	ops = purple_ssl_get_ops();
	return (ops->write)(gsc, data, len);
}
