static void
conv_updated_cb(PurpleConversation *conv, PurpleConvUpdateType type) {
	PidginConversation *pconv = PIDGIN_CONVERSATION(conv);
	PidginWindow *win = pidgin_conv_get_window(pconv);
	if (type == PURPLE_CONV_UPDATE_UNSEEN && !pidgin_conv_is_hidden(pconv)
			&& pconv->unseen_state == PIDGIN_UNSEEN_NONE
			&& pidgin_conv_window_get_gtkconv_count(win) == 1) {
		GtkWidget *window = win->window;
		gboolean has_focus;
		g_object_get(G_OBJECT(window), "has-toplevel-focus", &has_focus, NULL);
		if (!has_focus || !purple_prefs_get_bool(OPT_WINTRANS_IM_ONFOCUS))
			set_conv_window_trans(NULL, win);
		if (g_signal_handler_find(G_OBJECT(window), G_SIGNAL_MATCH_FUNC,
				0, 0, NULL, G_CALLBACK(focus_conv_win_cb), NULL) == 0) {
			g_signal_connect(G_OBJECT(window), "focus_in_event",
				G_CALLBACK(focus_conv_win_cb), window);
			g_signal_connect(G_OBJECT(window), "focus_out_event",
				G_CALLBACK(focus_conv_win_cb), window);
		}
	}
}
