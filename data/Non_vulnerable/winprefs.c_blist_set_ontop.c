}
static void blist_set_ontop(gboolean val) {
	if(!blist)
		return;
	gtk_window_set_keep_above(GTK_WINDOW(PIDGIN_BLIST(purple_get_blist())->window), val);
}
