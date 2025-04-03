}
void xmpp_disco_service_expand(XmppDiscoService *service)
{
	struct item_data *item_data;
	g_return_if_fail(service != NULL);
	if (service->expanded)
		return;
	item_data = g_new0(struct item_data, 1);
	item_data->list = service->list;
	item_data->parent = service;
	++service->list->fetch_count;
	pidgin_disco_list_ref(service->list);
	pidgin_disco_list_set_in_progress(service->list, TRUE);
	xmpp_disco_items_do(service->list->pc, item_data, service->jid, service->node,
	                    got_items_cb);
	service->expanded = TRUE;
}
