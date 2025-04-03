}
int tcl_cmd_connection(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *list, *elem;
	const char *cmds[] = { "account", "displayname", "handle", "list", "state", NULL };
	enum { CMD_CONN_ACCOUNT, CMD_CONN_DISPLAYNAME, CMD_CONN_HANDLE,
	       CMD_CONN_LIST, CMD_CONN_STATE } cmd;
	int error;
	GList *cur;
	PurpleConnection *gc;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_CONN_ACCOUNT:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "gc");
			return TCL_ERROR;
		}
		if ((gc = tcl_validate_gc(objv[2], interp)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
		                 purple_tcl_ref_new(PurpleTclRefAccount,
		                                  purple_connection_get_account(gc)));
		break;
	case CMD_CONN_DISPLAYNAME:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "gc");
			return TCL_ERROR;
		}
		if ((gc = tcl_validate_gc(objv[2], interp)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
                                 Tcl_NewStringObj(purple_connection_get_display_name(gc), -1));
		break;
	case CMD_CONN_HANDLE:
		if (objc != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "");
			return TCL_ERROR;
		}
		Tcl_SetObjResult(interp, purple_tcl_ref_new(PurpleTclRefHandle,
							    purple_connections_get_handle()));
		break;
	case CMD_CONN_LIST:
		if (objc != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "");
			return TCL_ERROR;
		}
		list = Tcl_NewListObj(0, NULL);
		for (cur = purple_connections_get_all(); cur != NULL; cur = g_list_next(cur)) {
			elem = purple_tcl_ref_new(PurpleTclRefConnection, cur->data);
			Tcl_ListObjAppendElement(interp, list, elem);
		}
		Tcl_SetObjResult(interp, list);
		break;
	case CMD_CONN_STATE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "gc");
			return TCL_ERROR;
		}
		if ((gc = tcl_validate_gc(objv[2], interp)) == NULL)
			return TCL_ERROR;
		switch (purple_connection_get_state(gc)) {
		case PURPLE_DISCONNECTED:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("disconnected", -1));
			break;
		case PURPLE_CONNECTED:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("connected", -1));
			break;
		case PURPLE_CONNECTING:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("connecting", -1));
			break;
		}
		break;
	}
	return TCL_OK;
}
