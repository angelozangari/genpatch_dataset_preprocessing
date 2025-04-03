void
purple_status_destroy(PurpleStatus *status)
{
	g_return_if_fail(status != NULL);
	g_hash_table_destroy(status->attr_values);
	PURPLE_DBUS_UNREGISTER_POINTER(status);
	g_free(status);
}
