static void
server_info_cb(PurpleConnection *pc, const char *type, const char *id,
               const char *from, xmlnode *iq, gpointer data)
{
	struct item_data *cb_data = data;
	PidginDiscoList *list = cb_data->list;
	xmlnode *query;
	xmlnode *error;
	gboolean items = FALSE;
	--list->fetch_count;
	if (g_str_equal(type, "result") &&
			(query = xmlnode_get_child(iq, "query"))) {
		xmlnode *feature;
		for (feature = xmlnode_get_child(query, "feature"); feature;
				feature = xmlnode_get_next_twin(feature)) {
			const char *var = xmlnode_get_attrib(feature, "var");
			if (purple_strequal(var, NS_DISCO_ITEMS)) {
				items = TRUE;
				break;
			}
		}
		if (items) {
			xmpp_disco_items_do(pc, cb_data, from, NULL /* node */, server_items_cb);
			++list->fetch_count;
			pidgin_disco_list_ref(list);
		}
		else {
			pidgin_disco_list_set_in_progress(list, FALSE);
			g_free(cb_data);
		}
	}
	else {
		error = xmlnode_get_child(iq, "error");
		if (xmlnode_get_child(error, "remote-server-not-found")
		 || xmlnode_get_child(error, "jid-malformed")) {
			purple_notify_error(my_plugin, _("Error"),
			                    _("Server does not exist"),
 			                    NULL);
		}
		else {
			purple_notify_error(my_plugin, _("Error"),
			                    _("Server does not support service discovery"),
			                    NULL);
		}
		pidgin_disco_list_set_in_progress(list, FALSE);
		g_free(cb_data);
	}
	pidgin_disco_list_unref(list);
}
