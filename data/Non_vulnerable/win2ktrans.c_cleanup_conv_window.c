/* Clean up transparency stuff for the conv window */
static void cleanup_conv_window(PidginWindow *win) {
	GtkWidget *window = win->window;
	slider_win *slidwin = NULL;
	/* Remove window from the window list */
	purple_debug_info(WINTRANS_PLUGIN_ID,
		"Conv window destroyed... removing from list\n");
	if ((slidwin = find_slidwin(window))) {
		window_list = g_slist_remove(window_list, slidwin);
		g_free(slidwin);
	}
	/* Remove the focus cbs */
	g_signal_handlers_disconnect_by_func(G_OBJECT(window),
		G_CALLBACK(focus_conv_win_cb), window);
}
