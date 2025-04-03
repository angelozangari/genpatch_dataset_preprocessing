static void
alert_config_cb(GtkWidget *widget, gpointer data)
{
	gboolean on = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	purple_prefs_set_bool("/plugins/gtk/unity/alert_chat_nick", on);
	alert_chat_nick = on;
}
