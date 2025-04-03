}
int tcl_cmd_status_attr(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	const char *cmds[] = { "id", "name", NULL };
	enum { CMD_STATUS_ATTR_ID, CMD_STATUS_ATTR_NAME } cmd;
	PurpleStatusAttr *attr;
	int error;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_STATUS_ATTR_ID:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "attr");
			return TCL_ERROR;
		}
		if ((attr = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusAttr)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj(purple_status_attr_get_id(attr), -1));
		break;
	case CMD_STATUS_ATTR_NAME:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "attr");
			return TCL_ERROR;
		}
		if ((attr = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusAttr)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj(purple_status_attr_get_name(attr), -1));
		break;
	}
	return TCL_OK;
}
