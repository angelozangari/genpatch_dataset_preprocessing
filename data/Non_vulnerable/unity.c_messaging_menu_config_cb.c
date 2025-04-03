static void
messaging_menu_config_cb(GtkWidget *widget, gpointer data)
{
	gint option = GPOINTER_TO_INT(data);
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		return;
	purple_prefs_set_int("/plugins/gtk/unity/messaging_menu_text", option);
	messaging_menu_text = option;
	refill_messaging_menu();
}
