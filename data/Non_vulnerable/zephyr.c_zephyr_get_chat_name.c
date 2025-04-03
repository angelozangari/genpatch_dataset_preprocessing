}
static char *zephyr_get_chat_name(GHashTable *data) {
	gchar* zclass = g_hash_table_lookup(data,"class");
	gchar* inst = g_hash_table_lookup(data,"instance");
	gchar* recipient = g_hash_table_lookup(data, "recipient");
	if (!zclass) /* This should never happen */
		zclass = "";
	if (!inst)
		inst = "*";
	if (!recipient)
		recipient = "";
	return g_strdup_printf("%s,%s,%s",zclass,inst,recipient);
}
