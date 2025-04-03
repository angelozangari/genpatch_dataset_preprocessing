static void
launcher_config_cb(GtkWidget *widget, gpointer data)
{
	gint option = GPOINTER_TO_INT(data);
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		return;
	purple_prefs_set_int("/plugins/gtk/unity/launcher_count", option);
	launcher_count = option;
	if (option == LAUNCHER_COUNT_DISABLE)
		unity_launcher_entry_set_count_visible(launcher, FALSE);
	else
		update_launcher();
}
