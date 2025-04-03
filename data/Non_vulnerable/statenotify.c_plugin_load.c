static gboolean
plugin_load(PurplePlugin *plugin)
{
	void *blist_handle = purple_blist_get_handle();
	purple_signal_connect(blist_handle, "buddy-status-changed", plugin,
	                    PURPLE_CALLBACK(buddy_status_changed_cb), NULL);
	purple_signal_connect(blist_handle, "buddy-idle-changed", plugin,
	                    PURPLE_CALLBACK(buddy_idle_changed_cb), NULL);
	purple_signal_connect(blist_handle, "buddy-signed-on", plugin,
	                    PURPLE_CALLBACK(buddy_signon_cb), NULL);
	purple_signal_connect(blist_handle, "buddy-signed-off", plugin,
	                    PURPLE_CALLBACK(buddy_signoff_cb), NULL);
	return TRUE;
}
