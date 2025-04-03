};
static void jabber_x_data_ok_cb(struct jabber_x_data_data *data, PurpleRequestFields *fields) {
	xmlnode *result = xmlnode_new("x");
	jabber_x_data_action_cb cb = data->cb;
	gpointer user_data = data->user_data;
	JabberStream *js = data->js;
	GList *groups, *flds;
	char *actionhandle = NULL;
	gboolean hasActions = (data->actions != NULL);
	xmlnode_set_namespace(result, "jabber:x:data");
	xmlnode_set_attrib(result, "type", "submit");
	for(groups = purple_request_fields_get_groups(fields); groups; groups = groups->next) {
		if(groups->data == data->actiongroup) {
			for(flds = purple_request_field_group_get_fields(groups->data); flds; flds = flds->next) {
				PurpleRequestField *field = flds->data;
				const char *id = purple_request_field_get_id(field);
				int handleindex;
				if(strcmp(id, "libpurple:jabber:xdata:actions"))
					continue;
				handleindex = purple_request_field_choice_get_value(field);
				actionhandle = g_strdup(g_list_nth_data(data->actions, handleindex));
				break;
			}
			continue;
		}
		for(flds = purple_request_field_group_get_fields(groups->data); flds; flds = flds->next) {
			xmlnode *fieldnode, *valuenode;
			PurpleRequestField *field = flds->data;
			const char *id = purple_request_field_get_id(field);
			jabber_x_data_field_type type = GPOINTER_TO_INT(g_hash_table_lookup(data->fields, id));
			switch(type) {
				case JABBER_X_DATA_TEXT_SINGLE:
				case JABBER_X_DATA_JID_SINGLE:
					{
					const char *value = purple_request_field_string_get_value(field);
					if (value == NULL)
						break;
					fieldnode = xmlnode_new_child(result, "field");
					xmlnode_set_attrib(fieldnode, "var", id);
					valuenode = xmlnode_new_child(fieldnode, "value");
					if(value)
						xmlnode_insert_data(valuenode, value, -1);
					break;
					}
				case JABBER_X_DATA_TEXT_MULTI:
					{
					char **pieces, **p;
					const char *value = purple_request_field_string_get_value(field);
					if (value == NULL)
						break;
					fieldnode = xmlnode_new_child(result, "field");
					xmlnode_set_attrib(fieldnode, "var", id);
					pieces = g_strsplit(value, "\n", -1);
					for(p = pieces; *p != NULL; p++) {
						valuenode = xmlnode_new_child(fieldnode, "value");
						xmlnode_insert_data(valuenode, *p, -1);
					}
					g_strfreev(pieces);
					}
					break;
				case JABBER_X_DATA_LIST_SINGLE:
				case JABBER_X_DATA_LIST_MULTI:
					{
					GList *selected = purple_request_field_list_get_selected(field);
					char *value;
					fieldnode = xmlnode_new_child(result, "field");
					xmlnode_set_attrib(fieldnode, "var", id);
					while(selected) {
						value = purple_request_field_list_get_data(field, selected->data);
						valuenode = xmlnode_new_child(fieldnode, "value");
						if(value)
							xmlnode_insert_data(valuenode, value, -1);
						selected = selected->next;
					}
					}
					break;
				case JABBER_X_DATA_BOOLEAN:
					fieldnode = xmlnode_new_child(result, "field");
					xmlnode_set_attrib(fieldnode, "var", id);
					valuenode = xmlnode_new_child(fieldnode, "value");
					if(purple_request_field_bool_get_value(field))
						xmlnode_insert_data(valuenode, "1", -1);
					else
						xmlnode_insert_data(valuenode, "0", -1);
					break;
				case JABBER_X_DATA_IGNORE:
					break;
			}
		}
	}
	g_hash_table_destroy(data->fields);
	while(data->values) {
		g_free(data->values->data);
		data->values = g_slist_delete_link(data->values, data->values);
	}
	if (data->actions) {
		GList *action;
		for(action = data->actions; action; action = g_list_next(action)) {
			g_free(action->data);
		}
		g_list_free(data->actions);
	}
	g_free(data);
	if (hasActions)
		cb(js, result, actionhandle, user_data);
	else
		((jabber_x_data_cb)cb)(js, result, user_data);
	g_free(actionhandle);
}
