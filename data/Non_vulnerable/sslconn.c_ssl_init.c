static gboolean
ssl_init(void)
{
	PurplePlugin *plugin;
	PurpleSslOps *ops;
	if (_ssl_initialized)
		return FALSE;
	plugin = purple_plugins_find_with_id("core-ssl");
	if (plugin != NULL && !purple_plugin_is_loaded(plugin))
		purple_plugin_load(plugin);
	ops = purple_ssl_get_ops();
	if ((ops == NULL) || (ops->init == NULL) || (ops->uninit == NULL) ||
		(ops->connectfunc == NULL) || (ops->close == NULL) ||
		(ops->read == NULL) || (ops->write == NULL))
	{
		return FALSE;
	}
	return (_ssl_initialized = ops->init());
}
void
purple_ssl_init(void)
{
	/* Although purple_ssl_is_supported will do the initialization on
	   command, SSL plugins tend to register CertificateSchemes as well
	   as providing SSL ops. */
	if (!ssl_init()) {
		purple_debug_error("sslconn", "Unable to initialize SSL.\n");
	}
}
