gboolean
purple_status_get_attr_boolean(const PurpleStatus *status, const char *id)
{
	const PurpleValue *value;
	g_return_val_if_fail(status != NULL, FALSE);
	g_return_val_if_fail(id     != NULL, FALSE);
	if ((value = purple_status_get_attr_value(status, id)) == NULL)
		return FALSE;
	g_return_val_if_fail(purple_value_get_type(value) == PURPLE_TYPE_BOOLEAN, FALSE);
	return purple_value_get_boolean(value);
}
