}
static TickerData *buddy_ticker_find_contact(PurpleContact *c) {
	GList *tb;
	for(tb = tickerbuds; tb; tb = tb->next) {
		TickerData *td = tb->data;
		if(td->contact == c)
			return td;
	}
	return NULL;
}
