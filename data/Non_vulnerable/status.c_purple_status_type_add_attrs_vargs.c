void
purple_status_type_add_attrs_vargs(PurpleStatusType *status_type, va_list args)
{
	const char *id, *name;
	PurpleValue *value;
	g_return_if_fail(status_type != NULL);
	while ((id = va_arg(args, const char *)) != NULL)
	{
		name = va_arg(args, const char *);
		g_return_if_fail(name != NULL);
		value = va_arg(args, PurpleValue *);
		g_return_if_fail(value != NULL);
		purple_status_type_add_attr(status_type, id, name, value);
	}
}
