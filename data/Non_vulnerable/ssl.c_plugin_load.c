static gboolean
plugin_load(PurplePlugin *plugin)
{
	return probe_ssl_plugins(plugin);
}
