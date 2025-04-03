}
int tcl_cmd_send_im(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	PurpleConnection *gc;
	char *who, *text;
	if (objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "gc who text");
		return TCL_ERROR;
	}
	if ((gc = tcl_validate_gc(objv[1], interp)) == NULL)
		return TCL_ERROR;
	who = Tcl_GetString(objv[2]);
	text = Tcl_GetString(objv[3]);
	serv_send_im(gc, who, text, 0);
	return TCL_OK;
}
