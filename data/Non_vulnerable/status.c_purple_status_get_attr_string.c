const char *
purple_status_get_attr_string(const PurpleStatus *status, const char *id)
{
	const PurpleValue *value;
	g_return_val_if_fail(status != NULL, NULL);
	g_return_val_if_fail(id     != NULL, NULL);
	if ((value = purple_status_get_attr_value(status, id)) == NULL)
		return NULL;
	g_return_val_if_fail(purple_value_get_type(value) == PURPLE_TYPE_STRING, NULL);
	return purple_value_get_string(value);
}
