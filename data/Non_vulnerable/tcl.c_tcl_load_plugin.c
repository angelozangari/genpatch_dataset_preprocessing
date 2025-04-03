}
static gboolean tcl_load_plugin(PurplePlugin *plugin)
{
	struct tcl_plugin_data *data;
	Tcl_Interp *interp;
	Tcl_Obj *result;
	plugin->extra = NULL;
	if ((interp = tcl_create_interp()) == NULL) {
		purple_debug(PURPLE_DEBUG_ERROR, "tcl", "Could not initialize Tcl interpreter\n");
		return FALSE;
	}
	Tcl_SourceRCFile(interp);
	if (Tcl_EvalFile(interp, plugin->path) != TCL_OK) {
		result = Tcl_GetObjResult(interp);
		purple_debug(PURPLE_DEBUG_ERROR, "tcl",
		           "Error evaluating %s: %s\n", plugin->path,
		           Tcl_GetString(result));
		Tcl_DeleteInterp(interp);
		return FALSE;
	}
	Tcl_Preserve((ClientData)interp);
	data = g_new0(struct tcl_plugin_data, 1);
	data->plugin = plugin;
	data->interp = interp;
	plugin->extra = data;
	g_hash_table_insert(tcl_plugins, (gpointer)interp, (gpointer)data);
	return TRUE;
}
