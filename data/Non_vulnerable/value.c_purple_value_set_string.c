void
purple_value_set_string(PurpleValue *value, const char *data)
{
	g_return_if_fail(value != NULL);
	g_return_if_fail(data == NULL || g_utf8_validate(data, -1, NULL));
	g_free(value->data.string_data);
	value->data.string_data = g_strdup(data);
}
