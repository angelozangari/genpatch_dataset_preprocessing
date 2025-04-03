unsigned int
purple_value_get_uint(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, 0);
	return value->data.int_data;
}
