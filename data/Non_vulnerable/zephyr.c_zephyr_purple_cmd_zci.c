}
static PurpleCmdRet zephyr_purple_cmd_zci(PurpleConversation *conv,
				      const char *cmd, char **args, char **error, void *data)
{
	/* args = class, instance, message */
	zephyr_account *zephyr = purple_conversation_get_gc(conv)->proto_data;
	if ( zephyr_send_message(zephyr,args[0],args[1],"",args[2],zephyr_get_signature(),""))
		return PURPLE_CMD_RET_OK;
	else
		return PURPLE_CMD_RET_FAILED;
}
