static GtkWidget *
get_config_frame(PurplePlugin *plugin)
{
	GtkWidget *ret = NULL, *frame = NULL;
	GtkWidget *vbox = NULL, *toggle = NULL;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width(GTK_CONTAINER (ret), 12);
	/* Alerts */
	frame = pidgin_make_frame(ret, _("Chatroom alerts"));
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	toggle = gtk_check_button_new_with_mnemonic(_("Chatroom message alerts _only where someone says your username"));
	gtk_box_pack_start(GTK_BOX(vbox), toggle, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
	                             purple_prefs_get_bool("/plugins/gtk/unity/alert_chat_nick"));
	g_signal_connect(G_OBJECT(toggle), "toggled",
	                 G_CALLBACK(alert_config_cb), NULL);
	/* Launcher integration */
	frame = pidgin_make_frame(ret, _("Launcher Icon"));
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	toggle = gtk_radio_button_new_with_mnemonic(NULL, _("_Disable launcher integration"));
	gtk_box_pack_start(GTK_BOX(vbox), toggle, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
		purple_prefs_get_int("/plugins/gtk/unity/launcher_count") == LAUNCHER_COUNT_DISABLE);
	g_signal_connect(G_OBJECT(toggle), "toggled",
	                 G_CALLBACK(launcher_config_cb), GUINT_TO_POINTER(LAUNCHER_COUNT_DISABLE));
	toggle = gtk_radio_button_new_with_mnemonic_from_widget(GTK_RADIO_BUTTON(toggle),
	                                                        _("Show number of unread _messages on launcher icon"));
	gtk_box_pack_start(GTK_BOX(vbox), toggle, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
		purple_prefs_get_int("/plugins/gtk/unity/launcher_count") == LAUNCHER_COUNT_MESSAGES);
	g_signal_connect(G_OBJECT(toggle), "toggled",
	                 G_CALLBACK(launcher_config_cb), GUINT_TO_POINTER(LAUNCHER_COUNT_MESSAGES));
	toggle = gtk_radio_button_new_with_mnemonic_from_widget(GTK_RADIO_BUTTON(toggle),
	                                                        _("Show number of unread co_nversations on launcher icon"));
	gtk_box_pack_start(GTK_BOX(vbox), toggle, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
		purple_prefs_get_int("/plugins/gtk/unity/launcher_count") == LAUNCHER_COUNT_SOURCES);
	g_signal_connect(G_OBJECT(toggle), "toggled",
	                 G_CALLBACK(launcher_config_cb), GUINT_TO_POINTER(LAUNCHER_COUNT_SOURCES));
	/* Messaging menu integration */
	frame = pidgin_make_frame(ret, _("Messaging Menu"));
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	toggle = gtk_radio_button_new_with_mnemonic(NULL,
	                                                        _("Show number of _unread messages for conversations in messaging menu"));
	gtk_box_pack_start(GTK_BOX(vbox), toggle, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
		purple_prefs_get_int("/plugins/gtk/unity/messaging_menu_text") == MESSAGING_MENU_COUNT);
	g_signal_connect(G_OBJECT(toggle), "toggled",
	                 G_CALLBACK(messaging_menu_config_cb), GUINT_TO_POINTER(MESSAGING_MENU_COUNT));
	toggle = gtk_radio_button_new_with_mnemonic_from_widget(GTK_RADIO_BUTTON(toggle),
	                                                        _("Show _elapsed time for unread conversations in messaging menu"));
	gtk_box_pack_start(GTK_BOX(vbox), toggle, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
		purple_prefs_get_int("/plugins/gtk/unity/messaging_menu_text") == MESSAGING_MENU_TIME);
	g_signal_connect(G_OBJECT(toggle), "toggled",
	                 G_CALLBACK(messaging_menu_config_cb), GUINT_TO_POINTER(MESSAGING_MENU_TIME));
	gtk_widget_show_all(ret);
	return ret;
}
