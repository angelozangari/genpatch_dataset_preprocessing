}
static void buddy_ticker_remove_buddy(PurpleBuddy *b) {
	PurpleContact *c = purple_buddy_get_contact(b);
	TickerData *td = buddy_ticker_find_contact(c);
	if (!td)
		return;
	purple_contact_invalidate_priority_buddy(c);
	/* pop up the ticker window again */
	buddy_ticker_create_window();
	buddy_ticker_update_contact(c);
}
