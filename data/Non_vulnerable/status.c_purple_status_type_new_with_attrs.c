PurpleStatusType *
purple_status_type_new_with_attrs(PurpleStatusPrimitive primitive,
		const char *id, const char *name,
		gboolean saveable, gboolean user_settable,
		gboolean independent, const char *attr_id,
		const char *attr_name, PurpleValue *attr_value,
		...)
{
	PurpleStatusType *status_type;
	va_list args;
	g_return_val_if_fail(primitive  != PURPLE_STATUS_UNSET, NULL);
	g_return_val_if_fail(attr_id    != NULL,              NULL);
	g_return_val_if_fail(attr_name  != NULL,              NULL);
	g_return_val_if_fail(attr_value != NULL,              NULL);
	status_type = purple_status_type_new_full(primitive, id, name, saveable,
			user_settable, independent);
	/* Add the first attribute */
	purple_status_type_add_attr(status_type, attr_id, attr_name, attr_value);
	va_start(args, attr_value);
	purple_status_type_add_attrs_vargs(status_type, args);
	va_end(args);
	return status_type;
}
