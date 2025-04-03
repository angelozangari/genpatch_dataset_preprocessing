static void
got_items_cb(PurpleConnection *pc, const char *type, const char *id,
             const char *from, xmlnode *iq, gpointer data)
{
	struct item_data *item_data = data;
	PidginDiscoList *list = item_data->list;
	xmlnode *query;
	gboolean has_items = FALSE;
	--list->fetch_count;
	if (!list->in_progress)
		goto out;
	if (g_str_equal(type, "result") &&
			(query = xmlnode_get_child(iq, "query"))) {
		xmlnode *item;
		for (item = xmlnode_get_child(query, "item"); item;
				item = xmlnode_get_next_twin(item)) {
			const char *jid = xmlnode_get_attrib(item, "jid");
			const char *name = xmlnode_get_attrib(item, "name");
			const char *node = xmlnode_get_attrib(item, "node");
			has_items = TRUE;
			if (item_data->parent->type == XMPP_DISCO_SERVICE_TYPE_CHAT) {
				/* This is a hacky first-order approximation. Any MUC
				 * component that has a >1 level hierarchy (a Yahoo MUC
				 * transport component probably does) will violate this.
				 *
				 * On the other hand, this is better than querying all the
				 * chats at conference.jabber.org to enumerate them.
				 */
				XmppDiscoService *service = g_new0(XmppDiscoService, 1);
				service->list = item_data->list;
				service->parent = item_data->parent;
				service->flags = XMPP_DISCO_ADD;
				service->type = XMPP_DISCO_SERVICE_TYPE_CHAT;
				service->name = g_strdup(name);
				service->jid = g_strdup(jid);
				service->node = g_strdup(node);
				pidgin_disco_add_service(list, service, item_data->parent);
			} else {
				struct item_data *item_data2 = g_new0(struct item_data, 1);
				item_data2->list = item_data->list;
				item_data2->parent = item_data->parent;
				item_data2->name = g_strdup(name);
				item_data2->node = g_strdup(node);
				++list->fetch_count;
				pidgin_disco_list_ref(list);
				xmpp_disco_info_do(pc, item_data2, jid, node, got_info_cb);
			}
		}
	}
	if (!has_items)
		pidgin_disco_add_service(list, NULL, item_data->parent);
out:
	if (list->fetch_count == 0)
		pidgin_disco_list_set_in_progress(list, FALSE);
	g_free(item_data);
	pidgin_disco_list_unref(list);
}
