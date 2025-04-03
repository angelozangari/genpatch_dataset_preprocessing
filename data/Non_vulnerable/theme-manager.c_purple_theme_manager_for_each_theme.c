void
purple_theme_manager_for_each_theme(PTFunc func)
{
	g_return_if_fail(func);
	g_hash_table_foreach(theme_table,
			(GHFunc) purple_theme_manager_function_wrapper, func);
}
