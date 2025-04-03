}
static gboolean unload_self(gpointer data)
{
	PurplePlugin *plugin = data;
	purple_plugin_unload(plugin);
	return FALSE;
}
