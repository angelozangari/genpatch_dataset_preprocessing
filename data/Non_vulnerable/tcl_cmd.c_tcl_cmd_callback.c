}
static PurpleCmdRet tcl_cmd_callback(PurpleConversation *conv, const gchar *cmd,
                                   gchar **args, gchar **errors,
                                   struct tcl_cmd_handler *handler)
{
	int retval, i;
	Tcl_Obj *command, *arg, *tclargs, *result;
	command = Tcl_NewListObj(0, NULL);
	Tcl_IncrRefCount(command);
	/* The callback */
	arg = Tcl_DuplicateObj(handler->namespace);
	Tcl_AppendStringsToObj(arg, "::cb", NULL);
	Tcl_ListObjAppendElement(handler->interp, command, arg);
	/* The conversation */
	arg = purple_tcl_ref_new(PurpleTclRefConversation, conv);
	Tcl_ListObjAppendElement(handler->interp, command, arg);
	/* The command */
	arg = Tcl_NewStringObj(cmd, -1);
	Tcl_ListObjAppendElement(handler->interp, command, arg);
	/* The args list */
	tclargs = Tcl_NewListObj(0, NULL);
	for (i = 0; i < handler->nargs; i++) {
		arg = Tcl_NewStringObj(args[i], -1);
		Tcl_ListObjAppendElement(handler->interp, tclargs, arg);
	}
	Tcl_ListObjAppendElement(handler->interp, command, tclargs);
	if (Tcl_EvalObjEx(handler->interp, command, TCL_EVAL_GLOBAL) != TCL_OK) {
		gchar *errorstr;
		errorstr = g_strdup_printf("error evaluating callback: %s\n",
		                           Tcl_GetString(Tcl_GetObjResult(handler->interp)));
		purple_debug(PURPLE_DEBUG_ERROR, "tcl", "%s", errorstr);
		*errors = errorstr;
		retval = PURPLE_CMD_RET_FAILED;
	} else {
		result = Tcl_GetObjResult(handler->interp);
		if (Tcl_GetIntFromObj(handler->interp, result,
		                      &retval) != TCL_OK) {
			gchar *errorstr;
			errorstr = g_strdup_printf("Error retreiving procedure result: %s\n",
			                           Tcl_GetString(Tcl_GetObjResult(handler->interp)));
			purple_debug(PURPLE_DEBUG_ERROR, "tcl", "%s", errorstr);
			*errors = errorstr;
			retval = PURPLE_CMD_RET_FAILED;
		}
	}
	return retval;
}
