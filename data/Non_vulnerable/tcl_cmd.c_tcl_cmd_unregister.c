}
void tcl_cmd_unregister(PurpleCmdId id, Tcl_Interp *interp)
{
	GList *cur;
	GString *cmd;
	gboolean found = FALSE;
	struct tcl_cmd_handler *handler;
	for (cur = tcl_cmd_callbacks; cur != NULL; cur = g_list_next(cur)) {
		handler = cur->data;
		if (handler->interp == interp && handler->id == id) {
			purple_cmd_unregister(id);
			cmd = g_string_sized_new(64);
			g_string_printf(cmd, "namespace delete %s",
			                Tcl_GetString(handler->namespace));
			Tcl_EvalEx(interp, cmd->str, -1, TCL_EVAL_GLOBAL);
			tcl_cmd_handler_free(handler);
			g_string_free(cmd, TRUE);
			cur->data = NULL;
			found = TRUE;
			break;
		}
	}
	if (found)
		tcl_cmd_callbacks = g_list_remove_all(tcl_cmd_callbacks, NULL);
}
