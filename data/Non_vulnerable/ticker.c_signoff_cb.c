static void
signoff_cb(PurpleConnection *gc)
{
	TickerData *td;
	if (!purple_connections_get_all()) {
		while (tickerbuds) {
			td = tickerbuds->data;
			tickerbuds = g_list_delete_link(tickerbuds, tickerbuds);
			if (td->timeout != 0)
				g_source_remove(td->timeout);
			g_free(td);
		}
		gtk_widget_destroy(tickerwindow);
		tickerwindow = NULL;
		ticker = NULL;
	} else {
		GList *t = tickerbuds;
		while (t) {
			td = t->data;
			t = t->next;
			buddy_ticker_update_contact(td->contact);
		}
	}
}
