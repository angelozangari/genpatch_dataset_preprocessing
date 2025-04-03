gboolean
purple_ssl_is_supported(void)
{
#ifdef HAVE_SSL
	ssl_init();
	return (purple_ssl_get_ops() != NULL);
#else
	return FALSE;
#endif
}
