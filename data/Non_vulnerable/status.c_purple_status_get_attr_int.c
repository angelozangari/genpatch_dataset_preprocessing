int
purple_status_get_attr_int(const PurpleStatus *status, const char *id)
{
	const PurpleValue *value;
	g_return_val_if_fail(status != NULL, 0);
	g_return_val_if_fail(id     != NULL, 0);
	if ((value = purple_status_get_attr_value(status, id)) == NULL)
		return 0;
	g_return_val_if_fail(purple_value_get_type(value) == PURPLE_TYPE_INT, 0);
	return purple_value_get_int(value);
}
