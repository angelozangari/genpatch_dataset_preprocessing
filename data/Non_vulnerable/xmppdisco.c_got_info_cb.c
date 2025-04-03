static void
got_info_cb(PurpleConnection *pc, const char *type, const char *id,
            const char *from, xmlnode *iq, gpointer data)
{
	struct item_data *item_data = data;
	PidginDiscoList *list = item_data->list;
	xmlnode *query;
	--list->fetch_count;
	if (!list->in_progress)
		goto out;
	if (g_str_equal(type, "result") &&
			(query = xmlnode_get_child(iq, "query"))) {
		xmlnode *identity = xmlnode_get_child(query, "identity");
		XmppDiscoService *service;
		xmlnode *feature;
		service = g_new0(XmppDiscoService, 1);
		service->list = item_data->list;
		purple_debug_info("xmppdisco", "parent for %s is %p\n", from, item_data->parent);
		service->parent = item_data->parent;
		service->flags = 0;
		service->type = disco_service_type_from_identity(identity);
		if (item_data->node) {
			if (item_data->name) {
				service->name = item_data->name;
				item_data->name = NULL;
			} else
				service->name = g_strdup(item_data->node);
			service->node = item_data->node;
			item_data->node = NULL;
			if (service->type == XMPP_DISCO_SERVICE_TYPE_PUBSUB_COLLECTION)
				service->flags |= XMPP_DISCO_BROWSE;
		} else
			service->name = g_strdup(from);
		if (!service->node)
			/* Only support adding JIDs, not JID+node combos */
			service->flags |= XMPP_DISCO_ADD;
		if (item_data->name) {
			service->description = item_data->name;
			item_data->name = NULL;
		} else if (identity)
			service->description = g_strdup(xmlnode_get_attrib(identity, "name"));
		/* TODO: Overlap with service->name a bit */
		service->jid = g_strdup(from);
		for (feature = xmlnode_get_child(query, "feature"); feature;
				feature = xmlnode_get_next_twin(feature)) {
			const char *var;
			if (!(var = xmlnode_get_attrib(feature, "var")))
				continue;
			if (g_str_equal(var, NS_REGISTER))
				service->flags |= XMPP_DISCO_REGISTER;
			else if (g_str_equal(var, NS_DISCO_ITEMS))
				service->flags |= XMPP_DISCO_BROWSE;
			else if (g_str_equal(var, NS_MUC)) {
				service->flags |= XMPP_DISCO_BROWSE;
				service->type = XMPP_DISCO_SERVICE_TYPE_CHAT;
			}
		}
		if (service->type == XMPP_DISCO_SERVICE_TYPE_GATEWAY)
			service->gateway_type = g_strdup(disco_type_from_string(
					xmlnode_get_attrib(identity, "type")));
		pidgin_disco_add_service(list, service, service->parent);
	}
out:
	if (list->fetch_count == 0)
		pidgin_disco_list_set_in_progress(list, FALSE);
	g_free(item_data->name);
	g_free(item_data->node);
	g_free(item_data);
	pidgin_disco_list_unref(list);
}
