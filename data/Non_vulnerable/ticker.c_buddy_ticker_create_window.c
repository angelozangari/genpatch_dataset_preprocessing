}
static void buddy_ticker_create_window(void) {
	if(tickerwindow) {
		gtk_widget_show(tickerwindow);
		return;
	}
	tickerwindow = pidgin_create_window(_("Buddy Ticker"), 0, "ticker", TRUE);
	gtk_window_set_default_size(GTK_WINDOW(tickerwindow), 500, -1);
	g_signal_connect(G_OBJECT(tickerwindow), "delete_event",
			G_CALLBACK (buddy_ticker_destroy_window), NULL);
	ticker = gtk_ticker_new();
	gtk_ticker_set_spacing(GTK_TICKER(ticker), 20);
	gtk_container_add(GTK_CONTAINER(tickerwindow), ticker);
	gtk_ticker_set_interval(GTK_TICKER(ticker), 500);
	gtk_ticker_set_scootch(GTK_TICKER(ticker), 10);
	gtk_ticker_start_scroll(GTK_TICKER(ticker));
	gtk_widget_set_size_request(ticker, 1, -1);
	gtk_widget_show_all(tickerwindow);
}
}
static void buddy_ticker_create_window(void) {
	if(tickerwindow) {
		gtk_widget_show(tickerwindow);
		return;
	}
	tickerwindow = pidgin_create_window(_("Buddy Ticker"), 0, "ticker", TRUE);
	gtk_window_set_default_size(GTK_WINDOW(tickerwindow), 500, -1);
	g_signal_connect(G_OBJECT(tickerwindow), "delete_event",
			G_CALLBACK (buddy_ticker_destroy_window), NULL);
	ticker = gtk_ticker_new();
	gtk_ticker_set_spacing(GTK_TICKER(ticker), 20);
	gtk_container_add(GTK_CONTAINER(tickerwindow), ticker);
	gtk_ticker_set_interval(GTK_TICKER(ticker), 500);
	gtk_ticker_set_scootch(GTK_TICKER(ticker), 10);
	gtk_ticker_start_scroll(GTK_TICKER(ticker));
	gtk_widget_set_size_request(ticker, 1, -1);
	gtk_widget_show_all(tickerwindow);
}
