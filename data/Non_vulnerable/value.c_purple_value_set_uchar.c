void
purple_value_set_uchar(PurpleValue *value, unsigned char data)
{
	g_return_if_fail(value != NULL);
	value->data.uchar_data = data;
}
