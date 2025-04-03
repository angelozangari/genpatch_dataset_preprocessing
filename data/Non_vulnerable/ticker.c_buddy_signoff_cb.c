static void
buddy_signoff_cb(PurpleBuddy *b)
{
	buddy_ticker_remove_buddy(b);
	if(!tickerbuds)
		gtk_widget_hide(tickerwindow);
}
