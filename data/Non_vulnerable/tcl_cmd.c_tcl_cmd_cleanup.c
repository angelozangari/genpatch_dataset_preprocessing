}
void tcl_cmd_cleanup(Tcl_Interp *interp)
{
	GList *cur;
	struct tcl_cmd_handler *handler;
	for (cur = tcl_cmd_callbacks; cur != NULL; cur = g_list_next(cur)) {
		handler = cur->data;
		if (handler->interp == interp) {
			purple_cmd_unregister(handler->id);
			tcl_cmd_handler_free(handler);
			cur->data = NULL;
		}
	}
	tcl_cmd_callbacks = g_list_remove_all(tcl_cmd_callbacks, NULL);
}
