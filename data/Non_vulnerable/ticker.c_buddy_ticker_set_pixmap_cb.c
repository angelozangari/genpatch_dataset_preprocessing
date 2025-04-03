}
static gboolean buddy_ticker_set_pixmap_cb(gpointer data) {
	TickerData *td = data;
	if (g_list_find(tickerbuds, td) != NULL) {
		buddy_ticker_update_contact(td->contact);
		td->timeout = 0;
	}
	return FALSE;
}
