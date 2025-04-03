}
static gboolean tcl_unload_plugin(PurplePlugin *plugin)
{
	struct tcl_plugin_data *data;
	if (plugin == NULL)
		return TRUE;
	data = plugin->extra;
	if (data != NULL) {
		g_hash_table_remove(tcl_plugins, (gpointer)(data->interp));
		purple_signals_disconnect_by_handle(data->interp);
		tcl_cmd_cleanup(data->interp);
		tcl_signal_cleanup(data->interp);
		Tcl_Release((ClientData)data->interp);
		Tcl_DeleteInterp(data->interp);
		g_free(data);
	}
	return TRUE;
}
