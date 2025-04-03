void
purple_theme_manager_uninit(void)
{
	g_hash_table_destroy(theme_table);
}
