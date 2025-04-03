}
static void buddy_ticker_update_contact(PurpleContact *contact) {
	TickerData *td = buddy_ticker_find_contact(contact);
	if (!td)
		return;
	/* pop up the ticker window again */
	buddy_ticker_create_window();
	if (purple_contact_get_priority_buddy(contact) == NULL)
		buddy_ticker_remove(td);
	else {
		buddy_ticker_set_pixmap(contact);
		gtk_label_set_text(GTK_LABEL(td->label), purple_contact_get_alias(contact));
	}
}
