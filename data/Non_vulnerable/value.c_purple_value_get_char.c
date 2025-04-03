char
purple_value_get_char(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, 0);
	return value->data.char_data;
}
