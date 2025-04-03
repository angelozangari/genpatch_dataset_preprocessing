void
purple_status_set_attr_string(PurpleStatus *status, const char *id,
		const char *value)
{
	PurpleValue *attr_value;
	g_return_if_fail(status != NULL);
	g_return_if_fail(id     != NULL);
	/* Make sure this attribute exists and is the correct type. */
	attr_value = purple_status_get_attr_value(status, id);
	/* This used to be g_return_if_fail, but it's failing a LOT, so
	 * let's generate a log error for now. */
	/* g_return_if_fail(attr_value != NULL); */
	if (attr_value == NULL) {
		purple_debug_error("status",
				 "Attempted to set status attribute '%s' for "
				 "status '%s', which is not legal.  Fix "
                                 "this!\n", id,
				 purple_status_type_get_name(purple_status_get_type(status)));
		return;
	}
	g_return_if_fail(purple_value_get_type(attr_value) == PURPLE_TYPE_STRING);
	/* XXX: Check if the value has actually changed. If it has, and the status
	 * is active, should this trigger 'status_has_changed'? */
	purple_value_set_string(attr_value, value);
}
