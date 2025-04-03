}
PurpleCmdId tcl_cmd_register(struct tcl_cmd_handler *handler)
{
	int id;
	GString *proc;
	if ((id = purple_cmd_register(Tcl_GetString(handler->cmd),
				    handler->args, handler->priority,
				    handler->flags, handler->prpl_id,
				    PURPLE_CMD_FUNC(tcl_cmd_callback),
				    handler->helpstr, (void *)handler)) == 0)
		return 0;
	handler->namespace = new_cmd_cb_namespace ();
	Tcl_IncrRefCount(handler->namespace);
	proc = g_string_new("");
	g_string_append_printf(proc, "namespace eval %s { proc cb { conv cmd arglist } { %s } }",
	                       Tcl_GetString(handler->namespace),
	                       Tcl_GetString(handler->proc));
	if (Tcl_Eval(handler->interp, proc->str) != TCL_OK) {
		Tcl_DecrRefCount(handler->namespace);
		g_string_free(proc, TRUE);
		return 0;
	}
	g_string_free(proc, TRUE);
	tcl_cmd_callbacks = g_list_append(tcl_cmd_callbacks, (gpointer)handler);
	return id;
}
