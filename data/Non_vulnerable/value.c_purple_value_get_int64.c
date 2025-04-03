gint64
purple_value_get_int64(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, 0);
	return value->data.int64_data;
}
