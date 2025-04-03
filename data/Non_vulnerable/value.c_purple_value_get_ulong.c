unsigned long
purple_value_get_ulong(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, 0);
	return value->data.long_data;
}
