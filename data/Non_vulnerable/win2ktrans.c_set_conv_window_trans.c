}
static void set_conv_window_trans(PidginWindow *oldwin, PidginWindow *newwin) {
	GtkWidget *win = newwin->window;
	/* check prefs to see if we want trans */
	if (purple_prefs_get_bool(OPT_WINTRANS_IM_ENABLED)) {
		set_wintrans(win, purple_prefs_get_int(OPT_WINTRANS_IM_ALPHA),
			TRUE, purple_prefs_get_bool(OPT_WINTRANS_IM_ONTOP));
		if (purple_prefs_get_bool(OPT_WINTRANS_IM_SLIDER)) {
			add_slider(win);
		}
	}
	/* If we're moving from one window to another,
	 * add the focus listeners to the new window if not already there */
	if (oldwin != NULL && oldwin != newwin) {
		if (pidgin_conv_window_get_gtkconv_count(newwin) == 0) {
			g_signal_connect(G_OBJECT(win), "focus_in_event",
				G_CALLBACK(focus_conv_win_cb), win);
			g_signal_connect(G_OBJECT(win), "focus_out_event",
				G_CALLBACK(focus_conv_win_cb), win);
		}
		/* If we've moved the last conversation, cleanup the window */
		if (pidgin_conv_window_get_gtkconv_count(oldwin) == 1)
			cleanup_conv_window(oldwin);
	}
}
