static void
messaging_menu_add_conversation(PurpleConversation *conv, gint count)
{
	gchar *id;
	g_return_if_fail(count > 0);
	id = conversation_id(conv);
	/* GBytesIcon may be useful for messaging menu source icons using buddy
	   icon data for IMs */
	if (!messaging_menu_app_has_source(mmapp, id))
		messaging_menu_app_append_source(mmapp, id, NULL,
		                                 purple_conversation_get_title(conv));
	if (messaging_menu_text == MESSAGING_MENU_TIME)
		messaging_menu_app_set_source_time(mmapp, id, g_get_real_time());
	else if (messaging_menu_text == MESSAGING_MENU_COUNT)
		messaging_menu_app_set_source_count(mmapp, id, count);
	messaging_menu_app_draw_attention(mmapp, id);
	g_free(id);
}
