}
gboolean tcl_signal_connect(struct tcl_signal_handler *handler)
{
	GString *proc;
	purple_signal_get_values(handler->instance,
			       Tcl_GetString(handler->signal),
			       &handler->returntype, &handler->nargs,
			       &handler->argtypes);
	tcl_signal_disconnect(handler->interp, Tcl_GetString(handler->signal),
			      handler->interp);
	if (!purple_signal_connect_vargs(handler->instance,
				       Tcl_GetString(handler->signal),
				       (void *)handler->interp,
				       PURPLE_CALLBACK(tcl_signal_callback),
				       (void *)handler))
		return FALSE;
	handler->namespace = new_cb_namespace ();
	Tcl_IncrRefCount(handler->namespace);
	proc = g_string_new("");
	g_string_append_printf(proc, "namespace eval %s { proc cb { %s } { %s } }",
			       Tcl_GetString(handler->namespace),
			       Tcl_GetString(handler->args),
	                       Tcl_GetString(handler->proc));
	if (Tcl_Eval(handler->interp, proc->str) != TCL_OK) {
		Tcl_DecrRefCount(handler->namespace);
		g_string_free(proc, TRUE);
		return FALSE;
	}
	g_string_free(proc, TRUE);
	tcl_callbacks = g_list_append(tcl_callbacks, (gpointer)handler);
	return TRUE;
}
