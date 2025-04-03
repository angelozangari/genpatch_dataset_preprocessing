}
static void tcl_destroy_plugin(PurplePlugin *plugin)
{
	if (plugin->info != NULL) {
		g_free(plugin->info->id);
		g_free(plugin->info->name);
		g_free(plugin->info->version);
		g_free(plugin->info->description);
		g_free(plugin->info->author);
		g_free(plugin->info->homepage);
	}
	return;
}
