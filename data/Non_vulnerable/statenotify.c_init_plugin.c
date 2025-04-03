static void
init_plugin(PurplePlugin *plugin)
{
	purple_prefs_add_none("/plugins/core/statenotify");
	purple_prefs_add_bool("/plugins/core/statenotify/notify_away", TRUE);
	purple_prefs_add_bool("/plugins/core/statenotify/notify_idle", TRUE);
	purple_prefs_add_bool("/plugins/core/statenotify/notify_signon", TRUE);
}
