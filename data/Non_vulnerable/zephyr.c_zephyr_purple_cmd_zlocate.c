}
static PurpleCmdRet zephyr_purple_cmd_zlocate(PurpleConversation *conv,
					  const char *cmd, char **args, char **error, void *data)
{
	zephyr_zloc(purple_conversation_get_gc(conv),args[0]);
	return PURPLE_CMD_RET_OK;
}
