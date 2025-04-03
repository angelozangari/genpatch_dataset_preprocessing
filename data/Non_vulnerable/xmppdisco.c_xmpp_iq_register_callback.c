static void
xmpp_iq_register_callback(PurpleConnection *pc, gchar *id, gpointer data,
                          XmppIqCallback cb)
{
	struct xmpp_iq_cb_data *cbdata = g_new0(struct xmpp_iq_cb_data, 1);
	cbdata->context = data;
	cbdata->cb = cb;
	cbdata->pc = pc;
	g_hash_table_insert(iq_callbacks, id, cbdata);
	if (!iq_listening) {
		PurplePlugin *prpl = purple_plugins_find_with_id(XMPP_PLUGIN_ID);
		iq_listening = TRUE;
		purple_signal_connect(prpl, "jabber-receiving-iq", my_plugin,
		                      PURPLE_CALLBACK(xmpp_iq_received), NULL);
	}
}
