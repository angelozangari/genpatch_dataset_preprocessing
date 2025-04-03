unsigned int
purple_value_get_subtype(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, 0);
	g_return_val_if_fail(purple_value_get_type(value) == PURPLE_TYPE_SUBTYPE, 0);
	return value->u.subtype;
}
