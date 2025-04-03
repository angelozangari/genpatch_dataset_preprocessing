}
int tcl_cmd_prefs(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *list, *elem, **elems;
	const char *cmds[] = { "get", "set", "type", NULL };
	enum { CMD_PREFS_GET, CMD_PREFS_SET, CMD_PREFS_TYPE } cmd;
	/* char *types[] = { "none", "boolean", "int", "string", "stringlist", NULL }; */
	/* enum { TCL_PREFS_NONE, TCL_PREFS_BOOL, TCL_PREFS_INT, TCL_PREFS_STRING, TCL_PREFS_STRINGLIST } type; */
	PurplePrefType preftype;
	GList *cur;
	int error, intval, nelem, i;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_PREFS_GET:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 1, objv, "path");
			return TCL_ERROR;
		}
		preftype = purple_prefs_get_type(Tcl_GetString(objv[2]));
		switch (preftype) {
		case PURPLE_PREF_NONE:
			Tcl_SetObjResult(interp,
					 Tcl_NewStringObj("pref type none", -1));
			return TCL_ERROR;
			break;
		case PURPLE_PREF_BOOLEAN:
			Tcl_SetObjResult(interp,
					 Tcl_NewBooleanObj(
						 purple_prefs_get_bool(Tcl_GetString(objv[2]))));
			break;
		case PURPLE_PREF_INT:
			Tcl_SetObjResult(interp, Tcl_NewIntObj(purple_prefs_get_int(Tcl_GetString(objv[2]))));
			break;
		case PURPLE_PREF_STRING:
			Tcl_SetObjResult(interp,
					 Tcl_NewStringObj((char *)purple_prefs_get_string(Tcl_GetString(objv[2])), -1));
			break;
		case PURPLE_PREF_STRING_LIST:
			cur = purple_prefs_get_string_list(Tcl_GetString(objv[2]));
			list = Tcl_NewListObj(0, NULL);
			while (cur != NULL) {
				elem = Tcl_NewStringObj((char *)cur->data, -1);
				Tcl_ListObjAppendElement(interp, list, elem);
				cur = g_list_next(cur);
			}
			Tcl_SetObjResult(interp, list);
			break;
		default:
			purple_debug(PURPLE_DEBUG_ERROR, "tcl", "tcl does not know about pref type %d\n", preftype);
			Tcl_SetObjResult(interp,
					 Tcl_NewStringObj("unknown pref type", -1));
			return TCL_ERROR;
		}
		break;
	case CMD_PREFS_SET:
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 1, objv, "path value");
			return TCL_ERROR;
		}
		preftype = purple_prefs_get_type(Tcl_GetString(objv[2]));
		switch (preftype) {
		case PURPLE_PREF_NONE:
			Tcl_SetObjResult(interp,
					 Tcl_NewStringObj("bad path or pref type none", -1));
			return TCL_ERROR;
			break;
		case PURPLE_PREF_BOOLEAN:
			if ((error = Tcl_GetBooleanFromObj(interp, objv[3], &intval)) != TCL_OK)
				return error;
			purple_prefs_set_bool(Tcl_GetString(objv[2]), intval);
			break;
		case PURPLE_PREF_INT:
			if ((error = Tcl_GetIntFromObj(interp, objv[3], &intval)) != TCL_OK)
				return error;
			purple_prefs_set_int(Tcl_GetString(objv[2]), intval);
			break;
		case PURPLE_PREF_STRING:
			purple_prefs_set_string(Tcl_GetString(objv[2]), Tcl_GetString(objv[3]));
			break;
		case PURPLE_PREF_STRING_LIST:
			if ((error = Tcl_ListObjGetElements(interp, objv[3], &nelem, &elems)) != TCL_OK)
				return error;
			cur = NULL;
			for (i = 0; i < nelem; i++) {
				cur = g_list_append(cur, (gpointer)Tcl_GetString(elems[i]));
			}
			purple_prefs_set_string_list(Tcl_GetString(objv[2]), cur);
			g_list_free(cur);
			break;
		default:
			purple_debug(PURPLE_DEBUG_ERROR, "tcl", "tcl does not know about pref type %d\n", preftype);
			return TCL_ERROR;
		}
		break;
	case CMD_PREFS_TYPE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 1, objv, "path");
			return TCL_ERROR;
		}
		preftype = purple_prefs_get_type(Tcl_GetString(objv[2]));
		switch (preftype) {
		case PURPLE_PREF_NONE:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("none", -1));
			break;
		case PURPLE_PREF_BOOLEAN:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("boolean", -1));
			break;
		case PURPLE_PREF_INT:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("int", -1));
			break;
		case PURPLE_PREF_STRING:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("string", -1));
			break;
		case PURPLE_PREF_STRING_LIST:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("stringlist", -1));
			break;
		default:
			purple_debug(PURPLE_DEBUG_ERROR, "tcl", "tcl does not know about pref type %d\n", preftype);
			Tcl_SetObjResult(interp, Tcl_NewStringObj("unknown", -1));
		}
		break;
	}
	return TCL_OK;
}
