static void
refill_messaging_menu()
{
	GList *convs;
	for (convs = purple_get_conversations(); convs != NULL; convs = convs->next) {
		PurpleConversation *conv = convs->data;
		messaging_menu_add_conversation(conv,
			GPOINTER_TO_INT(purple_conversation_get_data(conv, "unity-message-count")));
	}
}
