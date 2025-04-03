void
purple_value_set_ulong(PurpleValue *value, unsigned long data)
{
	g_return_if_fail(value != NULL);
	value->data.long_data = data;
}
