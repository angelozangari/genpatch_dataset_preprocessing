static void
new_conversation_cb(PurpleConversation *conv) {
	PidginWindow *win = pidgin_conv_get_window(PIDGIN_CONVERSATION(conv));
	/* If it is the first conversation in the window,
	 * add the sliders, and set transparency */
	if (!pidgin_conv_is_hidden(PIDGIN_CONVERSATION(conv)) && pidgin_conv_window_get_gtkconv_count(win) == 1) {
		GtkWidget *window = win->window;
		set_conv_window_trans(NULL, win);
		g_signal_connect(G_OBJECT(window), "focus_in_event",
			G_CALLBACK(focus_conv_win_cb), window);
		g_signal_connect(G_OBJECT(window), "focus_out_event",
			G_CALLBACK(focus_conv_win_cb), window);
	}
}
