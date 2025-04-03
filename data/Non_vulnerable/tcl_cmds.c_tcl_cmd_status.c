}
int tcl_cmd_status(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	const char *cmds[] = { "attr", "type", NULL };
	enum { CMD_STATUS_ATTR, CMD_STATUS_TYPE } cmd;
	PurpleStatus *status;
	PurpleStatusType *status_type;
	int error;
#if !(defined PURPLE_DISABLE_DEPRECATED)
	PurpleValue *value;
	const char *attr;
	int v;
#endif
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_STATUS_ATTR:
#if !(defined PURPLE_DISABLE_DEPRECATED)
		if (objc != 4 && objc != 5) {
			Tcl_WrongNumArgs(interp, 2, objv, "status attr_id ?value?");
			return TCL_ERROR;
		}
		if ((status = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatus)) == NULL)
			return TCL_ERROR;
		attr = Tcl_GetString(objv[3]);
		value = purple_status_get_attr_value(status, attr);
		if (value == NULL) {
			Tcl_SetObjResult(interp,
					 Tcl_NewStringObj("no such attribute", -1));
			return TCL_ERROR;
		}
		switch (purple_value_get_type(value)) {
		case PURPLE_TYPE_BOOLEAN:
			if (objc == 4) {
				Tcl_SetObjResult(interp,
						 Tcl_NewBooleanObj(purple_value_get_boolean(value)));
			} else {
				if ((error = Tcl_GetBooleanFromObj(interp, objv[4], &v)) != TCL_OK)
					return error;
				purple_status_set_attr_boolean(status, attr, v);
			}
			break;
		case PURPLE_TYPE_INT:
			if (objc == 4) {
				Tcl_SetObjResult(interp, Tcl_NewIntObj(purple_value_get_int(value)));
			} else {
				if ((error = Tcl_GetIntFromObj(interp, objv[4], &v)) != TCL_OK)
					return error;
				purple_status_set_attr_int(status, attr, v );
			}
			break;
		case PURPLE_TYPE_STRING:
			if (objc == 4)
				Tcl_SetObjResult(interp,
						 Tcl_NewStringObj(purple_value_get_string(value), -1));
			else
				purple_status_set_attr_string(status, attr, Tcl_GetString(objv[4]));
			break;
		default:
			Tcl_SetObjResult(interp,
                                         Tcl_NewStringObj("attribute has unknown type", -1));
			return TCL_ERROR;
		}
#endif
		break;
	case CMD_STATUS_TYPE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "status");
			return TCL_ERROR;
		}
		if ((status = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatus)) == NULL)
			return TCL_ERROR;
		status_type = purple_status_get_type(status);
		Tcl_SetObjResult(interp, purple_tcl_ref_new(PurpleTclRefStatusType,
		                                          status_type));
		break;
	}
	return TCL_OK;
}
