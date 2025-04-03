}
int tcl_cmd_debug(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	char *category, *message;
	int lev;
	const char *levels[] = { "-misc", "-info", "-warning", "-error", NULL };
	PurpleDebugLevel levelind[] = { PURPLE_DEBUG_MISC, PURPLE_DEBUG_INFO, PURPLE_DEBUG_WARNING, PURPLE_DEBUG_ERROR };
	int error;
	if (objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "level category message");
		return TCL_ERROR;
	}
	error = Tcl_GetIndexFromObj(interp, objv[1], levels, "debug level", 0, &lev);
	if (error != TCL_OK)
		return error;
	category = Tcl_GetString(objv[2]);
	message = Tcl_GetString(objv[3]);
	purple_debug(levelind[lev], category, "%s\n", message);
	return TCL_OK;
}
