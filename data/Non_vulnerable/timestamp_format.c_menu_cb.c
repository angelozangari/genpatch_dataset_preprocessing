static void
menu_cb(GtkWidget *item, gpointer data)
{
	PurplePlugin *plugin = data;
	GtkWidget *frame = pidgin_plugin_get_config_frame(plugin), *dialog;
	if (!frame)
		return;
	dialog = gtk_dialog_new_with_buttons(PIDGIN_ALERT_TITLE, NULL,
			GTK_DIALOG_NO_SEPARATOR | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
			NULL);
	g_signal_connect_after(G_OBJECT(dialog), "response", G_CALLBACK(gtk_widget_destroy), dialog);
#if GTK_CHECK_VERSION(2,14,0)
	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), frame);
#else
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), frame);
#endif
	gtk_window_set_role(GTK_WINDOW(dialog), "plugin_config");
	gtk_window_set_title(GTK_WINDOW(dialog), _(purple_plugin_get_name(plugin)));
	gtk_widget_show_all(dialog);
}
