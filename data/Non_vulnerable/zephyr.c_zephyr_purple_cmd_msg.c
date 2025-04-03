/*  commands */
static PurpleCmdRet zephyr_purple_cmd_msg(PurpleConversation *conv,
				      const char *cmd, char **args, char **error, void *data)
{
	char *recipient;
	zephyr_account *zephyr = purple_conversation_get_gc(conv)->proto_data;
	if (!g_ascii_strcasecmp(args[0],"*"))
		return PURPLE_CMD_RET_FAILED;  /* "*" is not a valid argument */
	else
		recipient = local_zephyr_normalize(zephyr,args[0]);
	if (strlen(recipient) < 1)
		return PURPLE_CMD_RET_FAILED; /* a null recipient is a chat message, not an IM */
	if (zephyr_send_message(zephyr,"MESSAGE","PERSONAL",recipient,args[1],zephyr_get_signature(),""))
		return PURPLE_CMD_RET_OK;
	else
		return PURPLE_CMD_RET_FAILED;
}
