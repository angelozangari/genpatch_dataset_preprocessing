static void
purple_theme_manager_function_wrapper(gchar *key,
		gpointer value,
		PTFunc user_data)
{
	if (PURPLE_IS_THEME(value))
		(* user_data)(value);
}
