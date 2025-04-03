static void
show_config(PurplePluginAction *action)
{
	if (!window) {
		FrameCreateCb create_frame = action->user_data;
		GtkWidget *vbox = gtk_vbox_new(FALSE, PIDGIN_HIG_BORDER);
		GtkWidget *hbox = gtk_hbox_new(FALSE, PIDGIN_HIG_BORDER);
		GtkWidget *config_frame = create_frame(NULL);
		GtkWidget *close = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
		gtk_container_add(GTK_CONTAINER(vbox), config_frame);
		gtk_container_add(GTK_CONTAINER(vbox), hbox);
		window = pidgin_create_window(action->label,
			PIDGIN_HIG_BORDER, NULL, FALSE);
		g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(config_destroy), NULL);
		g_signal_connect(G_OBJECT(close), "clicked",
		    G_CALLBACK(config_close), NULL);
		gtk_box_pack_end(GTK_BOX(hbox), close, FALSE, FALSE, PIDGIN_HIG_BORDER);
		gtk_container_add(GTK_CONTAINER(window), vbox);
		gtk_widget_show(GTK_WIDGET(close));
		gtk_widget_show(GTK_WIDGET(vbox));
		gtk_widget_show(GTK_WIDGET(hbox));
	}
	gtk_window_present(GTK_WINDOW(window));
}
