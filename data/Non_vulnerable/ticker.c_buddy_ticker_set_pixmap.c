}
static void buddy_ticker_set_pixmap(PurpleContact *c)
{
	TickerData *td = buddy_ticker_find_contact(c);
	PurpleBuddy *buddy;
	PurplePresence *presence;
	const char *stock;
	if(!td)
		return;
	buddy = purple_contact_get_priority_buddy(c);
	presence = purple_buddy_get_presence(buddy);
	stock = pidgin_stock_id_from_presence(presence);
	if(!td->icon) {
		td->icon = gtk_image_new();
		g_object_set(G_OBJECT(td->icon), "stock", stock,
				"icon-size", gtk_icon_size_from_name(PIDGIN_ICON_SIZE_TANGO_MICROSCOPIC),
				NULL);
	} else {
		g_object_set(G_OBJECT(td->icon), "stock", stock, NULL);
	}
}
