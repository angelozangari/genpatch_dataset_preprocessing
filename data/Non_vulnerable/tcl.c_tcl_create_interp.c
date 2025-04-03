}
static Tcl_Interp *tcl_create_interp(void)
{
	Tcl_Interp *interp;
	interp = Tcl_CreateInterp();
	if (Tcl_Init(interp) == TCL_ERROR) {
		Tcl_DeleteInterp(interp);
		return NULL;
	}
	if (tcl_init_interp(interp)) {
		Tcl_DeleteInterp(interp);
		return NULL;
	}
	Tcl_StaticPackage(interp, "purple", tcl_init_interp, NULL);
	return interp;
}
