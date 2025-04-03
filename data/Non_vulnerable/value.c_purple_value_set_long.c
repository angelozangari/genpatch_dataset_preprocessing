void
purple_value_set_long(PurpleValue *value, long data)
{
	g_return_if_fail(value != NULL);
	value->data.long_data = data;
}
