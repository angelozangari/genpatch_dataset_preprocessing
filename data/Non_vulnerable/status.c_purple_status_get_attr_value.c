PurpleValue *
purple_status_get_attr_value(const PurpleStatus *status, const char *id)
{
	g_return_val_if_fail(status != NULL, NULL);
	g_return_val_if_fail(id     != NULL, NULL);
	return (PurpleValue *)g_hash_table_lookup(status->attr_values, id);
}
