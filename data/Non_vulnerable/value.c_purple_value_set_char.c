void
purple_value_set_char(PurpleValue *value, char data)
{
	g_return_if_fail(value != NULL);
	value->data.char_data = data;
}
