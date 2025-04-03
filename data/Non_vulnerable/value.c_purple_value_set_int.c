void
purple_value_set_int(PurpleValue *value, int data)
{
	g_return_if_fail(value != NULL);
	value->data.int_data = data;
}
