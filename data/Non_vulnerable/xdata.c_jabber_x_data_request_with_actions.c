}
void *jabber_x_data_request_with_actions(JabberStream *js, xmlnode *packet, GList *actions, int defaultaction, jabber_x_data_action_cb cb, gpointer user_data)
{
	void *handle;
	xmlnode *fn, *x;
	PurpleRequestFields *fields;
	PurpleRequestFieldGroup *group;
	PurpleRequestField *field = NULL;
	char *title = NULL;
	char *instructions = NULL;
	struct jabber_x_data_data *data = g_new0(struct jabber_x_data_data, 1);
	data->fields = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	data->user_data = user_data;
	data->cb = cb;
	data->js = js;
	fields = purple_request_fields_new();
	group = purple_request_field_group_new(NULL);
	purple_request_fields_add_group(fields, group);
	for(fn = xmlnode_get_child(packet, "field"); fn; fn = xmlnode_get_next_twin(fn)) {
		xmlnode *valuenode;
		const char *type = xmlnode_get_attrib(fn, "type");
		const char *label = xmlnode_get_attrib(fn, "label");
		const char *var = xmlnode_get_attrib(fn, "var");
		char *value = NULL;
		if(!type)
			type = "text-single";
		if(!var && strcmp(type, "fixed"))
			continue;
		if(!label)
			label = var;
		if(!strcmp(type, "text-private")) {
			if((valuenode = xmlnode_get_child(fn, "value")))
				value = xmlnode_get_data(valuenode);
			field = purple_request_field_string_new(var, label,
					value ? value : "", FALSE);
			purple_request_field_string_set_masked(field, TRUE);
			purple_request_field_group_add_field(group, field);
			g_hash_table_replace(data->fields, g_strdup(var), GINT_TO_POINTER(JABBER_X_DATA_TEXT_SINGLE));
			g_free(value);
		} else if(!strcmp(type, "text-multi") || !strcmp(type, "jid-multi")) {
			GString *str = g_string_new("");
			for(valuenode = xmlnode_get_child(fn, "value"); valuenode;
					valuenode = xmlnode_get_next_twin(valuenode)) {
				if(!(value = xmlnode_get_data(valuenode)))
					continue;
				g_string_append_printf(str, "%s\n", value);
				g_free(value);
			}
			field = purple_request_field_string_new(var, label,
					str->str, TRUE);
			purple_request_field_group_add_field(group, field);
			g_hash_table_replace(data->fields, g_strdup(var), GINT_TO_POINTER(JABBER_X_DATA_TEXT_MULTI));
			g_string_free(str, TRUE);
		} else if(!strcmp(type, "list-single") || !strcmp(type, "list-multi")) {
			xmlnode *optnode;
			GList *selected = NULL;
			field = purple_request_field_list_new(var, label);
			if(!strcmp(type, "list-multi")) {
				purple_request_field_list_set_multi_select(field, TRUE);
				g_hash_table_replace(data->fields, g_strdup(var),
						GINT_TO_POINTER(JABBER_X_DATA_LIST_MULTI));
			} else {
				g_hash_table_replace(data->fields, g_strdup(var),
						GINT_TO_POINTER(JABBER_X_DATA_LIST_SINGLE));
			}
			for(valuenode = xmlnode_get_child(fn, "value"); valuenode;
					valuenode = xmlnode_get_next_twin(valuenode)) {
				char *data = xmlnode_get_data(valuenode);
				if (data != NULL) {
					selected = g_list_prepend(selected, data);
				}
			}
			for(optnode = xmlnode_get_child(fn, "option"); optnode;
					optnode = xmlnode_get_next_twin(optnode)) {
				const char *lbl;
				if(!(valuenode = xmlnode_get_child(optnode, "value")))
					continue;
				if(!(value = xmlnode_get_data(valuenode)))
					continue;
				if(!(lbl = xmlnode_get_attrib(optnode, "label")))
					lbl = value;
				data->values = g_slist_prepend(data->values, value);
				purple_request_field_list_add_icon(field, lbl, NULL, value);
				if(g_list_find_custom(selected, value, (GCompareFunc)strcmp))
					purple_request_field_list_add_selected(field, lbl);
			}
			purple_request_field_group_add_field(group, field);
			while(selected) {
				g_free(selected->data);
				selected = g_list_delete_link(selected, selected);
			}
		} else if(!strcmp(type, "boolean")) {
			gboolean def = FALSE;
			if((valuenode = xmlnode_get_child(fn, "value")))
				value = xmlnode_get_data(valuenode);
			if(value && (!g_ascii_strcasecmp(value, "yes") ||
						!g_ascii_strcasecmp(value, "true") || !g_ascii_strcasecmp(value, "1")))
				def = TRUE;
			field = purple_request_field_bool_new(var, label, def);
			purple_request_field_group_add_field(group, field);
			g_hash_table_replace(data->fields, g_strdup(var), GINT_TO_POINTER(JABBER_X_DATA_BOOLEAN));
			g_free(value);
		} else if(!strcmp(type, "fixed")) {
			if((valuenode = xmlnode_get_child(fn, "value")))
				value = xmlnode_get_data(valuenode);
			if(value != NULL) {
				field = purple_request_field_label_new("", value);
				purple_request_field_group_add_field(group, field);
				g_free(value);
			}
		} else if(!strcmp(type, "hidden")) {
			if((valuenode = xmlnode_get_child(fn, "value")))
				value = xmlnode_get_data(valuenode);
			field = purple_request_field_string_new(var, "", value ? value : "",
					FALSE);
			purple_request_field_set_visible(field, FALSE);
			purple_request_field_group_add_field(group, field);
			g_hash_table_replace(data->fields, g_strdup(var), GINT_TO_POINTER(JABBER_X_DATA_TEXT_SINGLE));
			g_free(value);
		} else { /* text-single, jid-single, and the default */
			if((valuenode = xmlnode_get_child(fn, "value")))
				value = xmlnode_get_data(valuenode);
			field = purple_request_field_string_new(var, label,
					value ? value : "", FALSE);
			purple_request_field_group_add_field(group, field);
			if(!strcmp(type, "jid-single")) {
				purple_request_field_set_type_hint(field, "screenname");
				g_hash_table_replace(data->fields, g_strdup(var), GINT_TO_POINTER(JABBER_X_DATA_JID_SINGLE));
			} else {
				g_hash_table_replace(data->fields, g_strdup(var), GINT_TO_POINTER(JABBER_X_DATA_TEXT_SINGLE));
			}
			g_free(value);
		}
		if(field && xmlnode_get_child(fn, "required"))
			purple_request_field_set_required(field,TRUE);
	}
	if(actions != NULL) {
		PurpleRequestField *actionfield;
		GList *action;
		data->actiongroup = group = purple_request_field_group_new(_("Actions"));
		purple_request_fields_add_group(fields, group);
		actionfield = purple_request_field_choice_new("libpurple:jabber:xdata:actions", _("Select an action"), defaultaction);
		for(action = actions; action; action = g_list_next(action)) {
			JabberXDataAction *a = action->data;
			purple_request_field_choice_add(actionfield, a->name);
			data->actions = g_list_append(data->actions, g_strdup(a->handle));
		}
		purple_request_field_set_required(actionfield,TRUE);
		purple_request_field_group_add_field(group, actionfield);
	}
	if((x = xmlnode_get_child(packet, "title")))
		title = xmlnode_get_data(x);
	if((x = xmlnode_get_child(packet, "instructions")))
		instructions = xmlnode_get_data(x);
	handle = purple_request_fields(js->gc, title, title, instructions, fields,
			_("OK"), G_CALLBACK(jabber_x_data_ok_cb),
			_("Cancel"), G_CALLBACK(jabber_x_data_cancel_cb),
			purple_connection_get_account(js->gc), /* XXX Do we have a who here? */ NULL, NULL,
			data);
	g_free(title);
	g_free(instructions);
	return handle;
}
