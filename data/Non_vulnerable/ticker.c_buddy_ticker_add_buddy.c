}
static void buddy_ticker_add_buddy(PurpleBuddy *b) {
	GtkWidget *hbox;
	TickerData *td;
	PurpleContact *contact;
	contact = purple_buddy_get_contact(b);
	buddy_ticker_create_window();
	if (!ticker)
		return;
	if (buddy_ticker_find_contact(contact))
	{
		buddy_ticker_update_contact(contact);
		return;
	}
	td = g_new0(TickerData, 1);
	td->contact = contact;
	tickerbuds = g_list_append(tickerbuds, td);
	td->ebox = gtk_event_box_new();
	gtk_ticker_add(GTK_TICKER(ticker), td->ebox);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(td->ebox), hbox);
	buddy_ticker_set_pixmap(contact);
	gtk_box_pack_start(GTK_BOX(hbox), td->icon, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(td->ebox), "button-press-event",
		G_CALLBACK(buddy_click_cb), contact);
	td->label = gtk_label_new(purple_contact_get_alias(contact));
	gtk_box_pack_start(GTK_BOX(hbox), td->label, FALSE, FALSE, 2);
	gtk_widget_show_all(td->ebox);
	gtk_widget_show(tickerwindow);
	/*
	 * Update the icon in a few seconds (after the open door icon has
	 * changed).  This is somewhat ugly.
	 */
	td->timeout = g_timeout_add(11000, buddy_ticker_set_pixmap_cb, td);
}
