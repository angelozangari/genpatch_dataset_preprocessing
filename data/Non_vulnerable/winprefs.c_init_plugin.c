static void
init_plugin(PurplePlugin *plugin)
{
	purple_prefs_add_none("/plugins/gtk");
	purple_prefs_add_none("/plugins/gtk/win32");
	purple_prefs_add_none("/plugins/gtk/win32/winprefs");
	purple_prefs_add_bool(PREF_DBLIST_DOCKABLE, FALSE);
	purple_prefs_add_bool(PREF_DBLIST_DOCKED, FALSE);
	purple_prefs_add_int(PREF_DBLIST_HEIGHT, 0);
	purple_prefs_add_int(PREF_DBLIST_SIDE, 0);
	/* Convert old preferences */
	if(purple_prefs_exists(PREF_DBLIST_ON_TOP)) {
		gint blist_top = BLIST_TOP_NEVER;
		if(purple_prefs_get_bool(PREF_BLIST_ON_TOP))
			blist_top = BLIST_TOP_ALWAYS;
		else if(purple_prefs_get_bool(PREF_DBLIST_ON_TOP))
			blist_top = BLIST_TOP_DOCKED;
		purple_prefs_remove(PREF_BLIST_ON_TOP);
		purple_prefs_remove(PREF_DBLIST_ON_TOP);
		purple_prefs_add_int(PREF_BLIST_ON_TOP, blist_top);
	} else
		purple_prefs_add_int(PREF_BLIST_ON_TOP, BLIST_TOP_NEVER);
	purple_prefs_remove(PREF_CHAT_BLINK);
}
