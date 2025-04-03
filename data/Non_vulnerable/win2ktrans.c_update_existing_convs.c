}
static void update_existing_convs() {
	GList *wins;
	for (wins = pidgin_conv_windows_get_list(); wins; wins = wins->next) {
		PidginWindow *win = wins->data;
		GtkWidget *window = win->window;
		set_conv_window_trans(NULL, win);
		g_signal_connect(G_OBJECT(window), "focus_in_event",
			G_CALLBACK(focus_conv_win_cb), window);
		g_signal_connect(G_OBJECT(window), "focus_out_event",
			G_CALLBACK(focus_conv_win_cb), window);
	}
}
