static void
get_plugin_frame(GtkWidget *parent, GtkSizeGroup *sg,
		const gchar *name, const gchar *plugin_label,
		const gchar **plugin_strs, const gchar *plugin_pref,
		const gchar *device_label, const gchar *device_pref)
{
	GtkWidget *vbox, *widget;
	GList *plugins, *devices;
	vbox = pidgin_make_frame(parent, name);
	/* Setup plugin preference */
	plugins = get_element_plugins(plugin_strs);
	widget = pidgin_prefs_dropdown_from_list(vbox, plugin_label,
			PURPLE_PREF_STRING, plugin_pref, plugins);
	g_list_free(plugins);
	gtk_size_group_add_widget(sg, widget);
	gtk_misc_set_alignment(GTK_MISC(widget), 0, 0.5);
	/* Setup device preference */
	devices = get_element_devices(purple_prefs_get_string(plugin_pref));
	if (g_list_find_custom(devices, purple_prefs_get_string(device_pref),
			(GCompareFunc) strcmp) == NULL)
		purple_prefs_set_string(device_pref, g_list_next(devices)->data);
	widget = pidgin_prefs_dropdown_from_list(vbox, device_label,
			PURPLE_PREF_STRING, device_pref, devices);
	g_list_free(devices);
	gtk_widget_set_name(widget, plugin_pref);
	gtk_size_group_add_widget(sg, widget);
	gtk_misc_set_alignment(GTK_MISC(widget), 0, 0.5);
	purple_prefs_connect_callback(vbox, plugin_pref,
			device_changed_cb, sg);
	g_signal_connect_swapped(vbox, "destroy",
			G_CALLBACK(purple_prefs_disconnect_by_handle), vbox);
}
