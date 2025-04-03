}
static PurpleCmdRet zephyr_purple_cmd_joinchat_cir(PurpleConversation *conv,
					       const char *cmd, char **args, char **error, void *data)
{
	/* Join a new zephyr chat */
	GHashTable *triple = g_hash_table_new(NULL,NULL);
	g_hash_table_insert(triple,"class",args[0]);
	g_hash_table_insert(triple,"instance",args[1]);
	g_hash_table_insert(triple,"recipient",args[2]);
	zephyr_join_chat(purple_conversation_get_gc(conv),triple);
	return PURPLE_CMD_RET_OK;
}
