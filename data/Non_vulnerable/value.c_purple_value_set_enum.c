void
purple_value_set_enum(PurpleValue *value, int data)
{
	g_return_if_fail(value != NULL);
	value->data.enum_data = data;
}
