/* Remove all transparency related aspects from conversation windows */
static void remove_convs_wintrans(gboolean remove_signal) {
	GList *wins;
	for (wins = pidgin_conv_windows_get_list(); wins; wins = wins->next) {
		PidginWindow *win = wins->data;
		GtkWidget *window = win->window;
		if (purple_prefs_get_bool(OPT_WINTRANS_IM_ENABLED))
			set_wintrans(window, 0, FALSE, FALSE);
		/* Remove the focus cbs */
		if (remove_signal)
			g_signal_handlers_disconnect_by_func(G_OBJECT(window),
				G_CALLBACK(focus_conv_win_cb), window);
	}
	remove_sliders();
}
