#endif /* _WIN32 */
static void tcl_init_plugin(PurplePlugin *plugin)
{
#ifdef USE_TCL_STUBS
	Tcl_Interp *interp = NULL;
#endif
	_tcl_plugin = plugin;
#ifdef USE_TCL_STUBS
#ifdef _WIN32
	if(!tcl_win32_init())
		return;
#endif
	if(!(interp = Tcl_CreateInterp()))
		return;
	if(!Tcl_InitStubs(interp, TCL_VERSION, 0)) {
		purple_debug(PURPLE_DEBUG_ERROR, "tcl", "Tcl_InitStubs: %s\n", interp->result);
		return;
	}
#endif
	Tcl_FindExecutable("purple");
#if defined(USE_TK_STUBS) && defined(HAVE_TK)
	Tk_Init(interp);
	if(!Tk_InitStubs(interp, TK_VERSION, 0)) {
		purple_debug(PURPLE_DEBUG_ERROR, "tcl", "Error Tk_InitStubs: %s\n", interp->result);
		Tcl_DeleteInterp(interp);
		return;
	}
#endif
	tcl_loaded = TRUE;
#ifdef USE_TCL_STUBS
	Tcl_DeleteInterp(interp);
#endif
	tcl_loader_info.exts = g_list_append(tcl_loader_info.exts, "tcl");
}
