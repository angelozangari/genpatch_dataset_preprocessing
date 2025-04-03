}
int tcl_cmd_notify(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	int error, type;
	const char *opts[] = { "-error", "-warning", "-info", NULL };
	PurpleNotifyMsgType optind[] = { PURPLE_NOTIFY_MSG_ERROR, PURPLE_NOTIFY_MSG_WARNING, PURPLE_NOTIFY_MSG_INFO };
	char *title, *msg1, *msg2;
	if (objc < 4 || objc > 5) {
		Tcl_WrongNumArgs(interp, 1, objv, "?type? title primary secondary");
		return TCL_ERROR;
	}
	if (objc == 4) {
		type = 1;			/* Default to warning */
		title = Tcl_GetString(objv[1]);
		msg1 = Tcl_GetString(objv[2]);
		msg2 = Tcl_GetString(objv[3]);
	} else {
		error = Tcl_GetIndexFromObj(interp, objv[1], opts, "message type", 0, &type);
		if (error != TCL_OK)
			return error;
		title = Tcl_GetString(objv[2]);
		msg1 = Tcl_GetString(objv[3]);
		msg2 = Tcl_GetString(objv[4]);
	}
	purple_notify_message(_tcl_plugin, optind[type], title, msg1, msg2, NULL, NULL);
	return TCL_OK;
}
