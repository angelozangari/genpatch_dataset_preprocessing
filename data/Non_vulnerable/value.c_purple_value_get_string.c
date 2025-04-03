const char *
purple_value_get_string(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, NULL);
	return value->data.string_data;
}
