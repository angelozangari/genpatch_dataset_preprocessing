void
purple_value_set_int64(PurpleValue *value, gint64 data)
{
	g_return_if_fail(value != NULL);
	value->data.int64_data = data;
}
