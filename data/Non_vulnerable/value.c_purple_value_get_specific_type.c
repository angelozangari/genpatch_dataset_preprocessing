const char *
purple_value_get_specific_type(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, NULL);
	g_return_val_if_fail(purple_value_get_type(value) == PURPLE_TYPE_BOXED, NULL);
	return value->u.specific_type;
}
