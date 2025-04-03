void
purple_value_set_boxed(PurpleValue *value, void *data)
{
	g_return_if_fail(value != NULL);
	value->data.boxed_data = data;
}
