static void
message_source_activated(MessagingMenuApp *app, const gchar *id,
                         gpointer user_data)
{
	gchar **sections = g_strsplit(id, ":", 0);
	PurpleConversation *conv = NULL;
	PurpleAccount *account;
	PidginWindow *purplewin = NULL;
	PurpleConversationType conv_type;
	char *type     = sections[0];
	char *cname    = sections[1];
	char *aname    = sections[2];
	char *protocol = sections[3];
	conv_type = type[0] - '0';
	account = purple_accounts_find(aname, protocol);
	conv = purple_find_conversation_with_account(conv_type, cname, account);
	if (conv) {
		unalert(conv);
		purplewin = PIDGIN_CONVERSATION(conv)->win;
		pidgin_conv_window_switch_gtkconv(purplewin, PIDGIN_CONVERSATION(conv));
		gdk_window_focus(gtk_widget_get_window(purplewin->window), time(NULL));
	}
	g_strfreev (sections);
}
