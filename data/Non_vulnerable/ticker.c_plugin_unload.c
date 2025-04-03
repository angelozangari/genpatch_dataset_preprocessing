static gboolean
plugin_unload(PurplePlugin *plugin)
{
	TickerData *td;
	while (tickerbuds) {
		td = tickerbuds->data;
		tickerbuds = g_list_delete_link(tickerbuds, tickerbuds);
		if (td->timeout != 0)
			g_source_remove(td->timeout);
		g_free(td);
	}
	if (tickerwindow != NULL) {
		gtk_widget_destroy(tickerwindow);
		tickerwindow = NULL;
	}
	return TRUE;
}
