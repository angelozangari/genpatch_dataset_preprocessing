static gboolean
remove_iq_callbacks_by_pc(gpointer key, gpointer value, gpointer user_data)
{
	struct xmpp_iq_cb_data *cb_data = value;
	if (cb_data && cb_data->pc == user_data) {
		struct item_data *item_data = cb_data->context;
		if (item_data) {
			pidgin_disco_list_unref(item_data->list);
			g_free(item_data->name);
			g_free(item_data->node);
			g_free(item_data);
		}
		return TRUE;
	} else
		return FALSE;
}
