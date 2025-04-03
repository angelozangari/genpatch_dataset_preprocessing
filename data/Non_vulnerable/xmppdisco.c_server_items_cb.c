static void
server_items_cb(PurpleConnection *pc, const char *type, const char *id,
                const char *from, xmlnode *iq, gpointer data)
{
	struct item_data *cb_data = data;
	PidginDiscoList *list = cb_data->list;
	xmlnode *query;
	g_free(cb_data);
	--list->fetch_count;
	if (g_str_equal(type, "result") &&
			(query = xmlnode_get_child(iq, "query"))) {
		xmlnode *item;
		for (item = xmlnode_get_child(query, "item"); item;
				item = xmlnode_get_next_twin(item)) {
			const char *jid = xmlnode_get_attrib(item, "jid");
			const char *name = xmlnode_get_attrib(item, "name");
			const char *node = xmlnode_get_attrib(item, "node");
			struct item_data *item_data;
			if (!jid)
				continue;
			item_data = g_new0(struct item_data, 1);
			item_data->list = list;
			item_data->name = g_strdup(name);
			item_data->node = g_strdup(node);
			++list->fetch_count;
			pidgin_disco_list_ref(list);
			xmpp_disco_info_do(pc, item_data, jid, node, got_info_cb);
		}
	}
	if (list->fetch_count == 0)
		pidgin_disco_list_set_in_progress(list, FALSE);
	pidgin_disco_list_unref(list);
}
