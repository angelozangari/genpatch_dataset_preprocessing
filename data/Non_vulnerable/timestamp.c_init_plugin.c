static void
init_plugin(PurplePlugin *plugin)
{
	purple_prefs_add_none("/plugins/gtk/timestamp");
	purple_prefs_add_int("/plugins/gtk/timestamp/interval", interval * 1000);
}
