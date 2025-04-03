}
static GList *zephyr_chat_info(PurpleConnection * gc)
{
	GList *m = NULL;
	struct proto_chat_entry *pce;
	pce = g_new0(struct proto_chat_entry, 1);
	pce->label = _("_Class:");
	pce->identifier = "class";
	m = g_list_append(m, pce);
	pce = g_new0(struct proto_chat_entry, 1);
	pce->label = _("_Instance:");
	pce->identifier = "instance";
	m = g_list_append(m, pce);
	pce = g_new0(struct proto_chat_entry, 1);
	pce->label = _("_Recipient:");
	pce->identifier = "recipient";
	m = g_list_append(m, pce);
	return m;
}
