void
purple_value_set_pointer(PurpleValue *value, void *data)
{
	g_return_if_fail(value != NULL);
	value->data.pointer_data = data;
}
