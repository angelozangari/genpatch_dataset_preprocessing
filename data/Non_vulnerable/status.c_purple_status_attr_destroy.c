void
purple_status_attr_destroy(PurpleStatusAttr *attr)
{
	g_return_if_fail(attr != NULL);
	g_free(attr->id);
	g_free(attr->name);
	purple_value_destroy(attr->value_type);
	PURPLE_DBUS_UNREGISTER_POINTER(attr);
	g_free(attr);
}
