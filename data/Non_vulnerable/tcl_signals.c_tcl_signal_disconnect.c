}
void tcl_signal_disconnect(void *instance, const char *signal, Tcl_Interp *interp)
{
	GList *cur;
	struct tcl_signal_handler *handler;
	gboolean found = FALSE;
	GString *cmd;
	for (cur = tcl_callbacks; cur != NULL; cur = g_list_next(cur)) {
		handler = cur->data;
		if (handler->interp == interp && handler->instance == instance
		    && !strcmp(signal, Tcl_GetString(handler->signal))) {
			purple_signal_disconnect(instance, signal, handler->interp,
					       PURPLE_CALLBACK(tcl_signal_callback));
			cmd = g_string_sized_new(64);
			g_string_printf(cmd, "namespace delete %s",
					Tcl_GetString(handler->namespace));
			Tcl_EvalEx(interp, cmd->str, -1, TCL_EVAL_GLOBAL);
			tcl_signal_handler_free(handler);
			g_string_free(cmd, TRUE);
			cur->data = NULL;
			found = TRUE;
			break;
		}
	}
	if (found)
		tcl_callbacks = g_list_remove_all(tcl_callbacks, NULL);
}
