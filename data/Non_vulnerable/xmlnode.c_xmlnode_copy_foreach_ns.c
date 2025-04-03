static void
xmlnode_copy_foreach_ns(gpointer key, gpointer value, gpointer user_data)
{
	GHashTable *ret = (GHashTable *)user_data;
	g_hash_table_insert(ret, g_strdup(key), g_strdup(value));
}
