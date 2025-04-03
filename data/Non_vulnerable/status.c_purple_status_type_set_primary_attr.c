void
purple_status_type_set_primary_attr(PurpleStatusType *status_type, const char *id)
{
	g_return_if_fail(status_type != NULL);
	g_free(status_type->primary_attr_id);
	status_type->primary_attr_id = g_strdup(id);
}
