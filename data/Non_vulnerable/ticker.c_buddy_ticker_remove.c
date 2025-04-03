}
static void buddy_ticker_remove(TickerData *td) {
	gtk_ticker_remove(GTK_TICKER(ticker), td->ebox);
	tickerbuds = g_list_remove(tickerbuds, td);
	if (td->timeout != 0)
		g_source_remove(td->timeout);
	g_free(td);
}
