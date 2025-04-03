int
purple_value_get_enum(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, -1);
	return value->data.enum_data;
}
