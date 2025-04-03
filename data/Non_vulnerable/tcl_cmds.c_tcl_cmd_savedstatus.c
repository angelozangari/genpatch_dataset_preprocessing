}
int tcl_cmd_savedstatus(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *result;
	const char *cmds[] = { "current", "handle", NULL };
	enum { CMD_SAVEDSTATUS_CURRENT, CMD_SAVEDSTATUS_HANDLE } cmd;
	int error;
	PurpleSavedStatus *saved_status;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_SAVEDSTATUS_CURRENT:
		if (objc != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "");
			return TCL_ERROR;
		}
		if ((saved_status = purple_savedstatus_get_current()) == NULL)
			return TCL_ERROR;
		result = Tcl_NewListObj(0, NULL);
		Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(purple_savedstatus_get_title(saved_status), -1));
		Tcl_ListObjAppendElement(interp, result, Tcl_NewIntObj(purple_savedstatus_get_type(saved_status)));
		Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(purple_savedstatus_get_message(saved_status), -1));
		Tcl_SetObjResult(interp,result);
		break;
	case CMD_SAVEDSTATUS_HANDLE:
		if (objc != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "");
			return TCL_ERROR;
		}
		Tcl_SetObjResult(interp,
				 purple_tcl_ref_new(PurpleTclRefHandle,
						    purple_savedstatuses_get_handle()));
		break;
	}
	return TCL_OK;
}
