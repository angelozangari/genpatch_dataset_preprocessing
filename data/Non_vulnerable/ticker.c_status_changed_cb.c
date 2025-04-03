static void
status_changed_cb(PurpleBuddy *b, PurpleStatus *os, PurpleStatus *s)
{
	PurpleContact *c = purple_buddy_get_contact(b);
	if(buddy_ticker_find_contact(c))
		buddy_ticker_set_pixmap(c);
	else
		buddy_ticker_add_buddy(b);
}
