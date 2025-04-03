void
purple_status_type_add_attr(PurpleStatusType *status_type, const char *id,
		const char *name, PurpleValue *value)
{
	PurpleStatusAttr *attr;
	g_return_if_fail(status_type != NULL);
	g_return_if_fail(id          != NULL);
	g_return_if_fail(name        != NULL);
	g_return_if_fail(value       != NULL);
	attr = purple_status_attr_new(id, name, value);
	status_type->attrs = g_list_append(status_type->attrs, attr);
}
