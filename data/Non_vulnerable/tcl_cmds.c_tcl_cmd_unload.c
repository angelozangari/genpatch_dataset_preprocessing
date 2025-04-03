}
int tcl_cmd_unload(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	PurplePlugin *plugin;
	if (objc != 1) {
		Tcl_WrongNumArgs(interp, 1, objv, "");
		return TCL_ERROR;
	}
	if ((plugin = tcl_interp_get_plugin(interp)) == NULL) {
		/* This isn't exactly OK, but heh.  What do you do? */
		return TCL_OK;
	}
	/* We can't unload immediately, but we can unload at the first
	 * known safe opportunity. */
	purple_timeout_add(0, unload_self, (gpointer)plugin);
	return TCL_OK;
}
