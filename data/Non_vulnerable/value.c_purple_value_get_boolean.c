gboolean
purple_value_get_boolean(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, FALSE);
	return value->data.boolean_data;
}
