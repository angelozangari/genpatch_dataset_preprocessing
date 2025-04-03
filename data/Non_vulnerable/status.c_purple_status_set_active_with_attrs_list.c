void
purple_status_set_active_with_attrs_list(PurpleStatus *status, gboolean active,
									   GList *attrs)
{
	gboolean changed = FALSE;
	GList *l;
	GList *specified_attr_ids = NULL;
	PurpleStatusType *status_type;
	g_return_if_fail(status != NULL);
	if (!active && purple_status_is_exclusive(status))
	{
		purple_debug_error("status",
				   "Cannot deactivate an exclusive status (%s).\n",
				   purple_status_get_id(status));
		return;
	}
	if (status->active != active)
	{
		changed = TRUE;
	}
	status->active = active;
	/* Set any attributes */
	l = attrs;
	while (l != NULL)
	{
		const gchar *id;
		PurpleValue *value;
		id = l->data;
		l = l->next;
		value = purple_status_get_attr_value(status, id);
		if (value == NULL)
		{
			purple_debug_warning("status", "The attribute \"%s\" on the status \"%s\" is "
							   "not supported.\n", id, status->type->name);
			/* Skip over the data and move on to the next attribute */
			l = l->next;
			continue;
		}
		specified_attr_ids = g_list_prepend(specified_attr_ids, (gpointer)id);
		if (value->type == PURPLE_TYPE_STRING)
		{
			const gchar *string_data = l->data;
			l = l->next;
			if (purple_strequal(string_data, value->data.string_data))
				continue;
			purple_status_set_attr_string(status, id, string_data);
			changed = TRUE;
		}
		else if (value->type == PURPLE_TYPE_INT)
		{
			int int_data = GPOINTER_TO_INT(l->data);
			l = l->next;
			if (int_data == value->data.int_data)
				continue;
			purple_status_set_attr_int(status, id, int_data);
			changed = TRUE;
		}
		else if (value->type == PURPLE_TYPE_BOOLEAN)
		{
			gboolean boolean_data = GPOINTER_TO_INT(l->data);
			l = l->next;
			if (boolean_data == value->data.boolean_data)
				continue;
			purple_status_set_attr_boolean(status, id, boolean_data);
			changed = TRUE;
		}
		else
		{
			/* We don't know what the data is--skip over it */
			l = l->next;
		}
	}
	/* Reset any unspecified attributes to their default value */
	status_type = purple_status_get_type(status);
	l = purple_status_type_get_attrs(status_type);
	while (l != NULL) {
		PurpleStatusAttr *attr;
		attr = l->data;
		l = l->next;
		if (!g_list_find_custom(specified_attr_ids, attr->id, (GCompareFunc)strcmp)) {
			PurpleValue *default_value;
			default_value = purple_status_attr_get_value(attr);
			if (default_value->type == PURPLE_TYPE_STRING) {
				const char *cur = purple_status_get_attr_string(status, attr->id);
				const char *def = purple_value_get_string(default_value);
				if ((cur == NULL && def == NULL)
				    || (cur != NULL && def != NULL
					&& !strcmp(cur, def))) {
					continue;
				}
				purple_status_set_attr_string(status, attr->id, def);
			} else if (default_value->type == PURPLE_TYPE_INT) {
				int cur = purple_status_get_attr_int(status, attr->id);
				int def = purple_value_get_int(default_value);
				if (cur == def)
					continue;
				purple_status_set_attr_int(status, attr->id, def);
			} else if (default_value->type == PURPLE_TYPE_BOOLEAN) {
				gboolean cur = purple_status_get_attr_boolean(status, attr->id);
				gboolean def = purple_value_get_boolean(default_value);
				if (cur == def)
					continue;
				purple_status_set_attr_boolean(status, attr->id, def);
			}
			changed = TRUE;
		}
	}
	g_list_free(specified_attr_ids);
	if (!changed)
		return;
	status_has_changed(status);
}
