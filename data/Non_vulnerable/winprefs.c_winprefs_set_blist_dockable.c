static void
winprefs_set_blist_dockable(const char *pref, PurplePrefType type,
		gconstpointer value, gpointer user_data)
{
	blist_set_dockable(GPOINTER_TO_INT(value));
}
