}
int tcl_cmd_signal(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	const char *cmds[] = { "connect", "disconnect", NULL };
	enum { CMD_SIGNAL_CONNECT, CMD_SIGNAL_DISCONNECT } cmd;
	struct tcl_signal_handler *handler;
	void *instance;
	int error;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_SIGNAL_CONNECT:
		if (objc != 6) {
			Tcl_WrongNumArgs(interp, 2, objv, "instance signal args proc");
			return TCL_ERROR;
		}
		handler = g_new0(struct tcl_signal_handler, 1);
		if ((handler->instance = purple_tcl_ref_get(interp, objv[2],PurpleTclRefHandle)) == NULL) {
			g_free(handler);
			return error;
		}
		handler->signal = objv[3];
		Tcl_IncrRefCount(handler->signal);
		handler->args = objv[4];
		handler->proc = objv[5];
		handler->interp = interp;
		if (!tcl_signal_connect(handler)) {
			tcl_signal_handler_free(handler);
			Tcl_SetObjResult(interp, Tcl_NewIntObj(1));
		} else {
			Tcl_SetObjResult(interp, Tcl_NewIntObj(0));
		}
		break;
	case CMD_SIGNAL_DISCONNECT:
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "instance signal");
			return TCL_ERROR;
		}
		if ((instance = purple_tcl_ref_get(interp, objv[2],PurpleTclRefHandle)) == NULL)
			return error;
		tcl_signal_disconnect(instance, Tcl_GetString(objv[3]), interp);
		break;
	}
	return TCL_OK;
}
