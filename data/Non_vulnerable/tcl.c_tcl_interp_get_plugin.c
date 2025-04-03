static gboolean tcl_loaded = FALSE;
PurplePlugin *tcl_interp_get_plugin(Tcl_Interp *interp)
{
	struct tcl_plugin_data *data;
	if (tcl_plugins == NULL)
		return NULL;
	data = g_hash_table_lookup(tcl_plugins, (gpointer)interp);
	return data != NULL ? data->plugin : NULL;
}
