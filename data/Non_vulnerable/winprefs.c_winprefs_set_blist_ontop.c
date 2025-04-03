static void
winprefs_set_blist_ontop(const char *pref, PurplePrefType type,
		gconstpointer value, gpointer user_data)
{
	gint setting = purple_prefs_get_int(PREF_BLIST_ON_TOP);
	if((setting == BLIST_TOP_DOCKED && blist_ab && blist_ab->docked)
		|| setting == BLIST_TOP_ALWAYS)
		blist_set_ontop(TRUE);
	else
		blist_set_ontop(FALSE);
}
