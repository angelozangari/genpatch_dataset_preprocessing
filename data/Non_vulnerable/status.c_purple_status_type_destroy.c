void
purple_status_type_destroy(PurpleStatusType *status_type)
{
	g_return_if_fail(status_type != NULL);
	g_free(status_type->id);
	g_free(status_type->name);
	g_free(status_type->primary_attr_id);
	g_list_foreach(status_type->attrs, (GFunc)purple_status_attr_destroy, NULL);
	g_list_free(status_type->attrs);
	PURPLE_DBUS_UNREGISTER_POINTER(status_type);
	g_free(status_type);
}
