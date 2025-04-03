}
static gboolean buddy_click_cb(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	PurpleContact *contact = user_data;
	PurpleBuddy *b = purple_contact_get_priority_buddy(contact);
	PurpleConversation *conv = purple_conversation_new(PURPLE_CONV_TYPE_IM,
	                                purple_buddy_get_account(b),
	                                purple_buddy_get_name(b));
	purple_conversation_present(conv);
	return TRUE;
}
