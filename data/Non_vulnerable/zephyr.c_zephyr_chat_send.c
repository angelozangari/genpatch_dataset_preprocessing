}
static int zephyr_chat_send(PurpleConnection * gc, int id, const char *im, PurpleMessageFlags flags)
{
	zephyr_triple *zt;
	const char *sig;
	PurpleConversation *gconv1;
	PurpleConvChat *gcc;
	char *inst;
	char *recipient;
	zephyr_account *zephyr = gc->proto_data;
	zt = find_sub_by_id(zephyr,id);
	if (!zt)
		/* this should never happen. */
		return -EINVAL;
	sig = zephyr_get_signature();
	gconv1 = purple_find_conversation_with_account(PURPLE_CONV_TYPE_CHAT, zt->name,
												 gc->account);
	gcc = purple_conversation_get_chat_data(gconv1);
	if (!(inst = (char *)purple_conv_chat_get_topic(gcc)))
		inst = g_strdup("PERSONAL");
	if (!g_ascii_strcasecmp(zt->recipient, "*"))
		recipient = local_zephyr_normalize(zephyr,"");
	else
		recipient = local_zephyr_normalize(zephyr,zt->recipient);
	zephyr_send_message(zephyr,zt->class,inst,recipient,im,sig,"");
	return 0;
}
