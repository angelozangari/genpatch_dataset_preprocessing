void
purple_status_set_attr_boolean(PurpleStatus *status, const char *id,
		gboolean value)
{
	PurpleValue *attr_value;
	g_return_if_fail(status != NULL);
	g_return_if_fail(id     != NULL);
	/* Make sure this attribute exists and is the correct type. */
	attr_value = purple_status_get_attr_value(status, id);
	g_return_if_fail(attr_value != NULL);
	g_return_if_fail(purple_value_get_type(attr_value) == PURPLE_TYPE_BOOLEAN);
	purple_value_set_boolean(attr_value, value);
}
