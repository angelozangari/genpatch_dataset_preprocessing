void
purple_value_set_short(PurpleValue *value, short data)
{
	g_return_if_fail(value != NULL);
	value->data.short_data = data;
}
