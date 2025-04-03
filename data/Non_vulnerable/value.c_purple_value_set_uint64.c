void
purple_value_set_uint64(PurpleValue *value, guint64 data)
{
	g_return_if_fail(value != NULL);
	value->data.uint64_data = data;
}
