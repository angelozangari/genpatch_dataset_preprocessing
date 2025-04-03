}
int tcl_cmd_status_type(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	const char *cmds[] = { "attr", "attrs", "available", "exclusive", "id",
	                       "independent", "name", "primary_attr",
	                       "primitive", "saveable", "user_settable",
	                       NULL };
	enum { CMD_STATUS_TYPE_ATTR, CMD_STATUS_TYPE_ATTRS,
	       CMD_STATUS_TYPE_AVAILABLE, CMD_STATUS_TYPE_EXCLUSIVE,
	       CMD_STATUS_TYPE_ID, CMD_STATUS_TYPE_INDEPENDENT,
	       CMD_STATUS_TYPE_NAME, CMD_STATUS_TYPE_PRIMARY_ATTR,
	       CMD_STATUS_TYPE_PRIMITIVE, CMD_STATUS_TYPE_SAVEABLE,
	       CMD_STATUS_TYPE_USER_SETTABLE } cmd;
	PurpleStatusType *status_type;
	Tcl_Obj *list, *elem;
	GList *cur;
	int error;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_STATUS_TYPE_AVAILABLE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewBooleanObj(purple_status_type_is_available(status_type)));
		break;
	case CMD_STATUS_TYPE_ATTR:
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype attr");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 purple_tcl_ref_new(PurpleTclRefStatusAttr,
						  purple_status_type_get_attr(status_type,
									    Tcl_GetStringFromObj(objv[3], NULL))));
		break;
	case CMD_STATUS_TYPE_ATTRS:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		list = Tcl_NewListObj(0, NULL);
		for (cur = purple_status_type_get_attrs(status_type);
		     cur != NULL; cur = g_list_next(cur)) {
			elem = purple_tcl_ref_new(PurpleTclRefStatusAttr, cur->data);
			Tcl_ListObjAppendElement(interp, list, elem);
		}
		Tcl_SetObjResult(interp, list);
		break;
	case CMD_STATUS_TYPE_EXCLUSIVE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewBooleanObj(purple_status_type_is_exclusive(status_type)));
		break;
	case CMD_STATUS_TYPE_ID:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj(purple_status_type_get_id(status_type), -1));
		break;
	case CMD_STATUS_TYPE_INDEPENDENT:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewBooleanObj(purple_status_type_is_independent(status_type)));
		break;
	case CMD_STATUS_TYPE_NAME:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj(purple_status_type_get_name(status_type), -1));
		break;
	case CMD_STATUS_TYPE_PRIMITIVE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj(purple_primitive_get_id_from_type
						  (purple_status_type_get_primitive(status_type)), -1));
		break;
	case CMD_STATUS_TYPE_PRIMARY_ATTR:
#if !(defined PURPLE_DISABLE_DEPRECATED)
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj(purple_status_type_get_primary_attr(status_type), -1));
#endif
		break;
	case CMD_STATUS_TYPE_SAVEABLE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewBooleanObj(
					 purple_status_type_is_saveable(status_type)));
		break;
	case CMD_STATUS_TYPE_USER_SETTABLE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "statustype");
			return TCL_ERROR;
		}
		if ((status_type = purple_tcl_ref_get(interp, objv[2], PurpleTclRefStatusType)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewBooleanObj(
					 purple_status_type_is_user_settable(status_type)));
		break;
	}
	return TCL_OK;
}
