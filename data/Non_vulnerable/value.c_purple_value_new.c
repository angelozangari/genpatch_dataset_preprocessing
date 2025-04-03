PurpleValue *
purple_value_new(PurpleType type, ...)
{
	PurpleValue *value;
	va_list args;
	g_return_val_if_fail(type != PURPLE_TYPE_UNKNOWN, NULL);
	value = g_new0(PurpleValue, 1);
	value->type = type;
	va_start(args, type);
	if (type == PURPLE_TYPE_SUBTYPE)
		value->u.subtype = va_arg(args, int);
	else if (type == PURPLE_TYPE_BOXED)
		value->u.specific_type = g_strdup(va_arg(args, char *));
	va_end(args);
	return value;
}
