static gboolean
plugin_load(PurplePlugin *plugin)
{
	PurplePlugin *jabber;
	jabber = purple_find_prpl("prpl-jabber");
	if (!jabber)
		return FALSE;
	xmpp_console_handle = plugin;
	purple_signal_connect(jabber, "jabber-receiving-xmlnode", xmpp_console_handle,
			    PURPLE_CALLBACK(xmlnode_received_cb), NULL);
	purple_signal_connect(jabber, "jabber-sending-text", xmpp_console_handle,
			    PURPLE_CALLBACK(xmlnode_sent_cb), NULL);
	purple_signal_connect(purple_connections_get_handle(), "signing-on",
			    plugin, PURPLE_CALLBACK(signing_on_cb), NULL);
	purple_signal_connect(purple_connections_get_handle(), "signed-off",
			    plugin, PURPLE_CALLBACK(signed_off_cb), NULL);
	return TRUE;
}
