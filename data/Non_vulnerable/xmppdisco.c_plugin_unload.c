static gboolean
plugin_unload(PurplePlugin *plugin)
{
	g_hash_table_destroy(iq_callbacks);
	iq_callbacks = NULL;
	purple_signals_disconnect_by_handle(plugin);
	pidgin_disco_dialogs_destroy_all();
	return TRUE;
}
