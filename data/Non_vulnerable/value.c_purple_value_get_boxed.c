void *
purple_value_get_boxed(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, NULL);
	return value->data.boxed_data;
}
