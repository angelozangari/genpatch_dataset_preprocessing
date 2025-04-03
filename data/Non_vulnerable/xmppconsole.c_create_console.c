static void
create_console(PurplePluginAction *action)
{
	GtkWidget *vbox = gtk_vbox_new(FALSE, 6);
	GtkWidget *label;
	GtkTextBuffer *buffer;
	GtkWidget *toolbar;
	GList *connections;
	GtkToolItem *button;
	if (console) {
		gtk_window_present(GTK_WINDOW(console->window));
		return;
	}
	console = g_new0(XmppConsole, 1);
	console->window = pidgin_create_window(_("XMPP Console"), PIDGIN_HIG_BORDER, NULL, TRUE);
	g_signal_connect(G_OBJECT(console->window), "destroy", G_CALLBACK(console_destroy), NULL);
	gtk_window_set_default_size(GTK_WINDOW(console->window), 580, 400);
	gtk_container_add(GTK_CONTAINER(console->window), vbox);
	console->hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), console->hbox, FALSE, FALSE, 0);
	label = gtk_label_new(_("Account: "));
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
	gtk_box_pack_start(GTK_BOX(console->hbox), label, FALSE, FALSE, 0);
	console->dropdown = gtk_combo_box_new_text();
	for (connections = purple_connections_get_all(); connections; connections = connections->next) {
		PurpleConnection *gc = connections->data;
		if (!strcmp(purple_account_get_protocol_id(purple_connection_get_account(gc)), "prpl-jabber")) {
			console->count++;
			console->accounts = g_list_append(console->accounts, gc);
			gtk_combo_box_append_text(GTK_COMBO_BOX(console->dropdown),
						  purple_account_get_username(purple_connection_get_account(gc)));
			if (!console->gc)
				console->gc = gc;
		}
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(console->dropdown),0);
	gtk_box_pack_start(GTK_BOX(console->hbox), console->dropdown, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(console->dropdown), "changed", G_CALLBACK(dropdown_changed_cb), NULL);
	console->imhtml = gtk_imhtml_new(NULL, NULL);
	if (console->count == 0)
		gtk_imhtml_append_text(GTK_IMHTML(console->imhtml),
				       _("<font color='#777777'>Not connected to XMPP</font>"), 0);
	gtk_box_pack_start(GTK_BOX(vbox), 
		pidgin_make_scrollable(console->imhtml, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC, GTK_SHADOW_ETCHED_IN, -1, -1),
		TRUE, TRUE, 0);
	toolbar = gtk_toolbar_new();
	button = gtk_tool_button_new(NULL, "<iq/>");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(iq_clicked_cb), NULL);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(button));
	button = gtk_tool_button_new(NULL, "<presence/>");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(presence_clicked_cb), NULL);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(button));
	button = gtk_tool_button_new(NULL, "<message/>");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(message_clicked_cb), NULL);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(button));
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
	console->entry = gtk_imhtml_new(NULL, NULL);
	gtk_imhtml_set_whole_buffer_formatting_only(GTK_IMHTML(console->entry), TRUE);
	g_signal_connect(G_OBJECT(console->entry),"message_send", G_CALLBACK(message_send_cb), console);
	console->sw = pidgin_make_scrollable(console->entry, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC, GTK_SHADOW_ETCHED_IN, -1, -1);
	gtk_box_pack_start(GTK_BOX(vbox), console->sw, FALSE, FALSE, 0);
	gtk_imhtml_set_editable(GTK_IMHTML(console->entry), TRUE);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console->entry));
	g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(entry_changed_cb), NULL);
	entry_changed_cb(buffer, NULL);
	gtk_widget_show_all(console->window);
	if (console->count < 2)
		gtk_widget_hide(console->hbox);
}
