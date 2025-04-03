void
purple_value_set_object(PurpleValue *value, void *data)
{
	g_return_if_fail(value != NULL);
	value->data.object_data = data;
}
