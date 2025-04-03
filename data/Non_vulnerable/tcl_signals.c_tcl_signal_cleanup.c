}
void tcl_signal_cleanup(Tcl_Interp *interp)
{
	GList *cur;
	struct tcl_signal_handler *handler;
	for (cur = tcl_callbacks; cur != NULL; cur = g_list_next(cur)) {
		handler = cur->data;
		if (handler->interp == interp) {
			tcl_signal_handler_free(handler);
			cur->data = NULL;
		}
	}
	tcl_callbacks = g_list_remove_all(tcl_callbacks, NULL);
}
