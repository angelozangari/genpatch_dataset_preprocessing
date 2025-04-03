 * hidden, it'll trigger the blist_visible_cb */
static gboolean listen_for_blist_visible_cb(gpointer data) {
	if (handle != NULL)
		blist_visible_cb_id =
			purple_prefs_connect_callback(handle,
				PIDGIN_PREFS_ROOT "/blist/list_visible",
				blist_visible_cb, NULL);
	return FALSE;
}
