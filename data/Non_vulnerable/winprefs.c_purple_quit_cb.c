   plugin is unloaded, when quitting */
static void purple_quit_cb() {
	purple_debug_info(WINPREFS_PLUGIN_ID, "purple_quit_cb: removing appbar\n");
	blist_save_state();
	blist_set_dockable(FALSE);
}
