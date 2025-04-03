PurpleType
purple_value_get_type(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, PURPLE_TYPE_UNKNOWN);
	return value->type;
}
