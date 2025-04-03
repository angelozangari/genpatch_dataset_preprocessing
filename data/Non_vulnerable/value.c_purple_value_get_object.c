void *
purple_value_get_object(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, NULL);
	return value->data.object_data;
}
