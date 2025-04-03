void
purple_value_set_ushort(PurpleValue *value, unsigned short data)
{
	g_return_if_fail(value != NULL);
	value->data.ushort_data = data;
}
