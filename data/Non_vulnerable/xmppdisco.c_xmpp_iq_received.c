static gboolean
xmpp_iq_received(PurpleConnection *pc, const char *type, const char *id,
                 const char *from, xmlnode *iq)
{
	struct xmpp_iq_cb_data *cb_data;
	cb_data = g_hash_table_lookup(iq_callbacks, id);
	if (!cb_data)
		return FALSE;
	cb_data->cb(cb_data->pc, type, id, from, iq, cb_data->context);
	g_hash_table_remove(iq_callbacks, id);
	if (g_hash_table_size(iq_callbacks) == 0) {
		PurplePlugin *prpl = purple_connection_get_prpl(pc);
		iq_listening = FALSE;
		purple_signal_disconnect(prpl, "jabber-receiving-iq", my_plugin,
		                         PURPLE_CALLBACK(xmpp_iq_received));
	}
	/* Om nom nom nom */
	return TRUE;
}
