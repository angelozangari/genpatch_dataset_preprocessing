static void
buddy_signon_cb(PurpleBuddy *b)
{
	PurpleContact *c = purple_buddy_get_contact(b);
	purple_contact_invalidate_priority_buddy(c);
	if(buddy_ticker_find_contact(c))
		buddy_ticker_update_contact(c);
	else
		buddy_ticker_add_buddy(b);
}
