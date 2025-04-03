}
static void zephyr_chat_set_topic(PurpleConnection * gc, int id, const char *topic)
{
	zephyr_triple *zt;
	PurpleConversation *gconv;
	PurpleConvChat *gcc;
	gchar *topic_utf8;
	zephyr_account* zephyr = gc->proto_data;
	char *sender = (char *)zephyr->username;
	zt = find_sub_by_id(zephyr,id);
	/* find_sub_by_id can return NULL */
	if (!zt)
		return;
	gconv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_CHAT, zt->name,
												gc->account);
	gcc = purple_conversation_get_chat_data(gconv);
	topic_utf8 = zephyr_recv_convert(gc,(gchar *)topic);
	purple_conv_chat_set_topic(gcc,sender,topic_utf8);
	g_free(topic_utf8);
	return;
}
