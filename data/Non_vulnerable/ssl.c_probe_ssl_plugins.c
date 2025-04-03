static gboolean
probe_ssl_plugins(PurplePlugin *my_plugin)
{
	PurplePlugin *plugin;
	GList *l;
	ssl_plugin = NULL;
	for (l = purple_plugins_get_all(); l != NULL; l = l->next)
	{
		plugin = (PurplePlugin *)l->data;
		if (plugin == my_plugin)
			continue;
		if (plugin->info != NULL && plugin->info->id != NULL &&
			strncmp(plugin->info->id, "ssl-", 4) == 0)
		{
			if (purple_plugin_is_loaded(plugin) || purple_plugin_load(plugin))
			{
				ssl_plugin = plugin;
				break;
			}
		}
	}
	return (ssl_plugin != NULL);
}
