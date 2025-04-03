}
int tcl_cmd_core(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	const char *cmds[] = { "handle", "quit", NULL };
	enum { CMD_CORE_HANDLE, CMD_CORE_QUIT } cmd;
	int error;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_CORE_HANDLE:
		if (objc != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "");
			return TCL_ERROR;
		}
		Tcl_SetObjResult(interp,
                                 purple_tcl_ref_new(PurpleTclRefHandle,
						    purple_get_core()));
		break;
	case CMD_CORE_QUIT:
		if (objc != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "");
			return TCL_ERROR;
		}
		purple_core_quit();
		break;
	}
	return TCL_OK;
}
