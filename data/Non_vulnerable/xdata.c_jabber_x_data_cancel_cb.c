}
static void jabber_x_data_cancel_cb(struct jabber_x_data_data *data, PurpleRequestFields *fields) {
	xmlnode *result = xmlnode_new("x");
	jabber_x_data_action_cb cb = data->cb;
	gpointer user_data = data->user_data;
	JabberStream *js = data->js;
	gboolean hasActions = FALSE;
	g_hash_table_destroy(data->fields);
	while(data->values) {
		g_free(data->values->data);
		data->values = g_slist_delete_link(data->values, data->values);
	}
	if (data->actions) {
		GList *action;
		hasActions = TRUE;
		for(action = data->actions; action; action = g_list_next(action)) {
			g_free(action->data);
		}
		g_list_free(data->actions);
	}
	g_free(data);
	xmlnode_set_namespace(result, "jabber:x:data");
	xmlnode_set_attrib(result, "type", "cancel");
	if (hasActions)
		cb(js, result, NULL, user_data);
	else
		((jabber_x_data_cb)cb)(js, result, user_data);
}
