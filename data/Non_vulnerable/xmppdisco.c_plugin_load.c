static gboolean
plugin_load(PurplePlugin *plugin)
{
	PurplePlugin *xmpp_prpl;
	my_plugin = plugin;
	xmpp_prpl = purple_plugins_find_with_id(XMPP_PLUGIN_ID);
	if (NULL == xmpp_prpl)
		return FALSE;
	purple_signal_connect(purple_connections_get_handle(), "signing-off",
	                      plugin, PURPLE_CALLBACK(signed_off_cb), NULL);
	iq_callbacks = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	return TRUE;
}
