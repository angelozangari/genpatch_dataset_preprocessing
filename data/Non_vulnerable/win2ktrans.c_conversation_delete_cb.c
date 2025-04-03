static void
conversation_delete_cb(PurpleConversation *conv) {
	PidginWindow *win = pidgin_conv_get_window(PIDGIN_CONVERSATION(conv));
	/* If it is the last conversation in the window, cleanup */
	if (win != NULL && pidgin_conv_window_get_gtkconv_count(win) == 1)
		cleanup_conv_window(win);
}
