void
purple_status_type_add_attrs(PurpleStatusType *status_type, const char *id,
		const char *name, PurpleValue *value, ...)
{
	va_list args;
	g_return_if_fail(status_type != NULL);
	g_return_if_fail(id          != NULL);
	g_return_if_fail(name        != NULL);
	g_return_if_fail(value       != NULL);
	/* Add the first attribute */
	purple_status_type_add_attr(status_type, id, name, value);
	va_start(args, value);
	purple_status_type_add_attrs_vargs(status_type, args);
	va_end(args);
}
