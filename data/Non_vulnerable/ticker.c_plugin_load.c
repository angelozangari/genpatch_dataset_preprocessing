static gboolean
plugin_load(PurplePlugin *plugin)
{
	void *blist_handle = purple_blist_get_handle();
	purple_signal_connect(purple_connections_get_handle(), "signed-off",
						plugin, PURPLE_CALLBACK(signoff_cb), NULL);
	purple_signal_connect(blist_handle, "buddy-signed-on",
						plugin, PURPLE_CALLBACK(buddy_signon_cb), NULL);
	purple_signal_connect(blist_handle, "buddy-signed-off",
						plugin, PURPLE_CALLBACK(buddy_signoff_cb), NULL);
	purple_signal_connect(blist_handle, "buddy-status-changed",
						plugin, PURPLE_CALLBACK(status_changed_cb), NULL);
	if (purple_connections_get_all())
		buddy_ticker_show();
	return TRUE;
}
