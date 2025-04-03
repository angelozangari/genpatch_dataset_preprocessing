static void
device_changed_cb(const gchar *name, PurplePrefType type,
		gconstpointer value, gpointer data)
{
	GtkSizeGroup *sg = data;
	GtkWidget *parent, *widget;
	GSList *widgets;
	GList *devices;
	GValue gvalue;
	gint position;
	gchar *label, *pref;
	widgets = gtk_size_group_get_widgets(GTK_SIZE_GROUP(sg));
	for (; widgets; widgets = g_slist_next(widgets)) {
		const gchar *widget_name =
				gtk_widget_get_name(GTK_WIDGET(widgets->data));
		if (!strcmp(widget_name, name)) {
			gchar *temp_str;
			gchar delimiters[3] = {0, 0, 0};
			const gchar *text;
			gint keyval, pos;
			widget = widgets->data;
			/* Get label with _ from the GtkLabel */
			text = gtk_label_get_text(GTK_LABEL(widget));
			keyval = gtk_label_get_mnemonic_keyval(GTK_LABEL(widget));
			delimiters[0] = g_ascii_tolower(keyval);
			delimiters[1] = g_ascii_toupper(keyval);
			pos = strcspn(text, delimiters);
			if (pos != -1) {
				temp_str = g_strndup(text, pos);
				label = g_strconcat(temp_str, "_",
						text + pos, NULL);
				g_free(temp_str);
			} else {
				label = g_strdup(text);
			}
			break;
		}
	}
	if (widgets == NULL)
		return;
	parent = gtk_widget_get_parent(widget);
	widget = parent;
	parent = gtk_widget_get_parent(GTK_WIDGET(widget));
	gvalue.g_type = 0;
	g_value_init(&gvalue, G_TYPE_INT);
	gtk_container_child_get_property(GTK_CONTAINER(parent),
			GTK_WIDGET(widget), "position", &gvalue);
	position = g_value_get_int(&gvalue);
	g_value_unset(&gvalue);
	gtk_widget_destroy(widget);
	pref = g_strdup(name);
	strcpy(pref + strlen(pref) - strlen("plugin"), "device");
	devices = get_element_devices(value);
	if (g_list_find_custom(devices, purple_prefs_get_string(pref),
			(GCompareFunc)strcmp) == NULL)
		purple_prefs_set_string(pref, g_list_next(devices)->data);
	widget = pidgin_prefs_dropdown_from_list(parent,
			label, PURPLE_PREF_STRING,
			pref, devices);
	g_list_free(devices);
	g_signal_connect_swapped(widget, "destroy",
			G_CALLBACK(g_free), pref);
	g_free(label);
	gtk_misc_set_alignment(GTK_MISC(widget), 0, 0.5);
	gtk_widget_set_name(widget, name);
	gtk_size_group_add_widget(sg, widget);
	gtk_box_reorder_child(GTK_BOX(parent),
			gtk_widget_get_parent(GTK_WIDGET(widget)), position);
}
