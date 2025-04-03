void *
purple_value_get_pointer(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, NULL);
	return value->data.pointer_data;
}
