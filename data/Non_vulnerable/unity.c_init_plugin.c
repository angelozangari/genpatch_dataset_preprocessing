static void
init_plugin(PurplePlugin *plugin)
{
	purple_prefs_add_none("/plugins/gtk");
	purple_prefs_add_none("/plugins/gtk/unity");
	purple_prefs_add_int("/plugins/gtk/unity/launcher_count", LAUNCHER_COUNT_SOURCES);
	purple_prefs_add_int("/plugins/gtk/unity/messaging_menu_text", MESSAGING_MENU_COUNT);
	purple_prefs_add_bool("/plugins/gtk/unity/alert_chat_nick", TRUE);
}
