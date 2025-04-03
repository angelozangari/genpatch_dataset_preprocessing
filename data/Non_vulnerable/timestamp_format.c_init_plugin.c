static void
init_plugin(PurplePlugin *plugin)
{
	purple_prefs_add_none("/plugins/gtk");
	purple_prefs_add_none("/plugins/gtk/timestamp_format");
	if (!purple_prefs_exists("/plugins/gtk/timestamp_format/force") &&
	    purple_prefs_exists("/plugins/gtk/timestamp_format/force_24hr"))
	{
		if (purple_prefs_get_bool(
		   "/plugins/gtk/timestamp_format/force_24hr"))
			purple_prefs_add_string("/plugins/gtk/timestamp_format/force", "force24");
		else
			purple_prefs_add_string("/plugins/gtk/timestamp_format/force", "default");
	}
	else
		purple_prefs_add_string("/plugins/gtk/timestamp_format/force", "default");
	purple_prefs_add_none("/plugins/gtk/timestamp_format/use_dates");
	purple_prefs_add_string("/plugins/gtk/timestamp_format/use_dates/conversation", "automatic");
	purple_prefs_add_string("/plugins/gtk/timestamp_format/use_dates/log", "automatic");
}
