long
purple_value_get_long(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, 0);
	return value->data.long_data;
}
