}
static PurpleCmdRet zephyr_purple_cmd_zcir(PurpleConversation *conv,
				       const char *cmd, char **args, char **error, void *data)
{
	/* args = class, instance, recipient, message */
	zephyr_account *zephyr = purple_conversation_get_gc(conv)->proto_data;
	if ( zephyr_send_message(zephyr,args[0],args[1],args[2],args[3],zephyr_get_signature(),""))
		return PURPLE_CMD_RET_OK;
	else
		return PURPLE_CMD_RET_FAILED;
}
