static void
init_plugin(PurplePlugin *plugin)
{
	purple_prefs_add_none("/plugins/gtk/win32");
	purple_prefs_add_none("/plugins/gtk/win32/wintrans");
	purple_prefs_add_bool(OPT_WINTRANS_IM_ENABLED, FALSE);
	purple_prefs_add_int(OPT_WINTRANS_IM_ALPHA, 255);
	purple_prefs_add_bool(OPT_WINTRANS_IM_SLIDER, FALSE);
	purple_prefs_add_bool(OPT_WINTRANS_IM_ONFOCUS, FALSE);
	purple_prefs_add_bool(OPT_WINTRANS_IM_ONTOP, FALSE);
	purple_prefs_add_bool(OPT_WINTRANS_BL_ENABLED, FALSE);
	purple_prefs_add_int(OPT_WINTRANS_BL_ALPHA, 255);
	purple_prefs_add_bool(OPT_WINTRANS_BL_ONFOCUS, FALSE);
	purple_prefs_add_bool(OPT_WINTRANS_BL_ONTOP, FALSE);
}
