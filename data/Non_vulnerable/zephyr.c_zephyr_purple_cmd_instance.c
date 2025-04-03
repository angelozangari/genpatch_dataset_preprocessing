}
static PurpleCmdRet zephyr_purple_cmd_instance(PurpleConversation *conv,
					   const char *cmd, char **args, char **error, void *data)
{
	/* Currently it sets the instance with leading spaces and
	 * all. This might not be the best thing to do, though having
	 * one word isn't ideal either.	 */
	PurpleConvChat *gcc = purple_conversation_get_chat_data(conv);
	int id = gcc->id;
	const char* instance = args[0];
	zephyr_chat_set_topic(purple_conversation_get_gc(conv),id,instance);
	return PURPLE_CMD_RET_OK;
}
