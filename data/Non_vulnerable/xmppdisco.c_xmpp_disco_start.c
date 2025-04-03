}
void xmpp_disco_start(PidginDiscoList *list)
{
	struct item_data *cb_data;
	g_return_if_fail(list != NULL);
	++list->fetch_count;
	pidgin_disco_list_ref(list);
	cb_data = g_new0(struct item_data, 1);
	cb_data->list = list;
	xmpp_disco_info_do(list->pc, cb_data, list->server, NULL, server_info_cb);
}
