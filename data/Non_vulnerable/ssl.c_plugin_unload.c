static gboolean
plugin_unload(PurplePlugin *plugin)
{
	if (ssl_plugin != NULL &&
		g_list_find(purple_plugins_get_loaded(), ssl_plugin) != NULL)
	{
		purple_plugin_unload(ssl_plugin);
	}
	ssl_plugin = NULL;
	return TRUE;
}
