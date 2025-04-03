void
purple_theme_manager_init(void)
{
	theme_table = g_hash_table_new_full(g_str_hash,
			g_str_equal, g_free, g_object_unref);
}
