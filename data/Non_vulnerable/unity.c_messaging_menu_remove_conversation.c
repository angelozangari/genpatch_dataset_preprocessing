static void
messaging_menu_remove_conversation(PurpleConversation *conv)
{
	gchar *id = conversation_id(conv);
	if (messaging_menu_app_has_source(mmapp, id))
		messaging_menu_app_remove_source(mmapp, id);
	g_free(id);
}
