short
purple_value_get_short(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, 0);
	return value->data.short_data;
}
