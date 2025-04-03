void
purple_value_set_boolean(PurpleValue *value, gboolean data)
{
	g_return_if_fail(value != NULL);
	value->data.boolean_data = data;
}
