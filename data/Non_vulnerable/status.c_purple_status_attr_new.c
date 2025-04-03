PurpleStatusAttr *
purple_status_attr_new(const char *id, const char *name, PurpleValue *value_type)
{
	PurpleStatusAttr *attr;
	g_return_val_if_fail(id         != NULL, NULL);
	g_return_val_if_fail(name       != NULL, NULL);
	g_return_val_if_fail(value_type != NULL, NULL);
	attr = g_new0(PurpleStatusAttr, 1);
	PURPLE_DBUS_REGISTER_POINTER(attr, PurpleStatusAttr);
	attr->id         = g_strdup(id);
	attr->name       = g_strdup(name);
	attr->value_type = value_type;
	return attr;
}
