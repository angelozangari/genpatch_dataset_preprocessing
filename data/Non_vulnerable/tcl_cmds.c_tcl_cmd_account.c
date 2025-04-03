}
int tcl_cmd_account(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *result, *list, *elem;
	const char *cmds[] = { "alias", "connect", "connection", "disconnect",
	                       "enabled", "find", "handle", "isconnected",
	                       "list", "presence", "protocol", "status",
	                       "status_type", "status_types", "username",
	                       NULL };
	enum { CMD_ACCOUNT_ALIAS,
	       CMD_ACCOUNT_CONNECT, CMD_ACCOUNT_CONNECTION,
	       CMD_ACCOUNT_DISCONNECT, CMD_ACCOUNT_ENABLED, CMD_ACCOUNT_FIND,
	       CMD_ACCOUNT_HANDLE, CMD_ACCOUNT_ISCONNECTED, CMD_ACCOUNT_LIST,
	       CMD_ACCOUNT_PRESENCE, CMD_ACCOUNT_PROTOCOL, CMD_ACCOUNT_STATUS,
	       CMD_ACCOUNT_STATUS_TYPE, CMD_ACCOUNT_STATUS_TYPES,
	       CMD_ACCOUNT_USERNAME } cmd;
	const char *listopts[] = { "-all", "-online", NULL };
	enum { CMD_ACCOUNTLIST_ALL, CMD_ACCOUNTLIST_ONLINE } listopt;
	const char *alias;
	GList *cur;
	PurpleAccount *account;
	PurpleStatus *status;
	PurpleStatusType *status_type;
	PurpleValue *value;
	char *attr_id;
	int error;
	int b, i;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_ACCOUNT_ALIAS:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		alias = purple_account_get_alias(account);
		Tcl_SetObjResult(interp, Tcl_NewStringObj(alias ? (char *)alias : "", -1));
		break;
	case CMD_ACCOUNT_CONNECT:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		if (!purple_account_is_connected(account))
			purple_account_connect(account);
		Tcl_SetObjResult(interp,
		                 purple_tcl_ref_new(PurpleTclRefConnection,
		                                  purple_account_get_connection(account)));
		break;
	case CMD_ACCOUNT_CONNECTION:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
		                 purple_tcl_ref_new(PurpleTclRefConnection,
						    purple_account_get_connection(account)));
		break;
	case CMD_ACCOUNT_DISCONNECT:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		purple_account_disconnect(account);
		break;
	case CMD_ACCOUNT_ENABLED:
		if (objc != 3 && objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "account ?enabled?");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		if (objc == 3) {
			Tcl_SetObjResult(interp,
					 Tcl_NewBooleanObj(
						 purple_account_get_enabled(account,
									    purple_core_get_ui())));
		} else {
			if ((error = Tcl_GetBooleanFromObj(interp, objv[3], &b)) != TCL_OK)
				return TCL_ERROR;
			purple_account_set_enabled(account, purple_core_get_ui(), b);
		}
		break;
	case CMD_ACCOUNT_FIND:
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "username protocol");
			return TCL_ERROR;
		}
		account = purple_accounts_find(Tcl_GetString(objv[2]),
		                             Tcl_GetString(objv[3]));
		Tcl_SetObjResult(interp,
		                 purple_tcl_ref_new(PurpleTclRefAccount, account));
		break;
	case CMD_ACCOUNT_HANDLE:
		if (objc != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "");
			return TCL_ERROR;
		}
		Tcl_SetObjResult(interp,
				 purple_tcl_ref_new(PurpleTclRefHandle,
						    purple_accounts_get_handle()));
		break;
	case CMD_ACCOUNT_ISCONNECTED:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewBooleanObj(
					 purple_account_is_connected(account)));
		break;
	case CMD_ACCOUNT_LIST:
		listopt = CMD_ACCOUNTLIST_ALL;
		if (objc > 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "?option?");
			return TCL_ERROR;
		}
		if (objc == 3) {
			if ((error = Tcl_GetIndexFromObj(interp, objv[2], listopts, "option", 0, (int *)&listopt)) != TCL_OK)
				return error;
		}
		list = Tcl_NewListObj(0, NULL);
		for (cur = purple_accounts_get_all(); cur != NULL; cur = g_list_next(cur)) {
			account = cur->data;
			if (listopt == CMD_ACCOUNTLIST_ONLINE && !purple_account_is_connected(account))
				continue;
			elem = purple_tcl_ref_new(PurpleTclRefAccount, account);
			Tcl_ListObjAppendElement(interp, list, elem);
		}
		Tcl_SetObjResult(interp, list);
		break;
	case CMD_ACCOUNT_PRESENCE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp, purple_tcl_ref_new(PurpleTclRefPresence,
							  purple_account_get_presence(account)));
		break;
	case CMD_ACCOUNT_PROTOCOL:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp, Tcl_NewStringObj((char *)purple_account_get_protocol_id(account), -1));
		break;
	case CMD_ACCOUNT_STATUS:
		if (objc < 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account ?status_id name value ...?");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		if (objc == 3) {
			Tcl_SetObjResult(interp,
					 purple_tcl_ref_new(PurpleTclRefStatus,
							  purple_account_get_active_status(account)));
		} else {
			GList *l = NULL;
			if (objc % 2) {
				Tcl_SetObjResult(interp, Tcl_NewStringObj("name without value setting status", -1));
				return TCL_ERROR;
			}
			status = purple_account_get_status(account, Tcl_GetString(objv[3]));
			if (status == NULL) {
				Tcl_SetObjResult(interp, Tcl_NewStringObj("invalid status for account", -1));
				return TCL_ERROR;
			}
			for (i = 4; i < objc; i += 2) {
				attr_id = Tcl_GetString(objv[i]);
				value = purple_status_get_attr_value(status, attr_id);
				if (value == NULL) {
					Tcl_SetObjResult(interp, Tcl_NewStringObj("invalid attribute for account", -1));
					return TCL_ERROR;
				}
				switch (purple_value_get_type(value)) {
				case PURPLE_TYPE_BOOLEAN:
					error = Tcl_GetBooleanFromObj(interp, objv[i + 1], &b);
					if (error != TCL_OK)
						return error;
					l = g_list_append(l, attr_id);
					l = g_list_append(l, GINT_TO_POINTER(b));
					break;
				case PURPLE_TYPE_INT:
					error = Tcl_GetIntFromObj(interp, objv[i + 1], &b);
					if (error != TCL_OK)
						return error;
					l = g_list_append(l, attr_id);
					l = g_list_append(l, GINT_TO_POINTER(b));
					break;
				case PURPLE_TYPE_STRING:
					l = g_list_append(l, attr_id);
					l = g_list_append(l, Tcl_GetString(objv[i + 1]));
					break;
				default:
					Tcl_SetObjResult(interp, Tcl_NewStringObj("unknown PurpleValue type", -1));
					return TCL_ERROR;
				}
			}
			purple_account_set_status_list(account, Tcl_GetString(objv[3]), TRUE, l);
			g_list_free(l);
		}
		break;
	case CMD_ACCOUNT_STATUS_TYPE:
		if (objc != 4 && objc != 5) {
			Tcl_WrongNumArgs(interp, 2, objv, "account ?statustype? ?-primitive primitive?");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		if (objc == 4) {
			status_type = purple_account_get_status_type(account,
								   Tcl_GetString(objv[3]));
		} else {
			PurpleStatusPrimitive primitive;
			if (strcmp(Tcl_GetString(objv[3]), "-primitive")) {
				result = Tcl_NewStringObj("bad option \"", -1);
				Tcl_AppendObjToObj(result, objv[3]);
				Tcl_AppendToObj(result, "\": should be -primitive", -1);
				Tcl_SetObjResult(interp,result);
				return TCL_ERROR;
			}
			primitive = purple_primitive_get_type_from_id(Tcl_GetString(objv[4]));
			status_type = purple_account_get_status_type_with_primitive(account,
										  primitive);
		}
		if (status_type == NULL) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("status type not found", -1));
			return TCL_ERROR;
		}
		Tcl_SetObjResult(interp,
				 purple_tcl_ref_new(PurpleTclRefStatusType,
						  status_type));
		break;
	case CMD_ACCOUNT_STATUS_TYPES:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		list = Tcl_NewListObj(0, NULL);
		for (cur = purple_account_get_status_types(account); cur != NULL;
		     cur = g_list_next(cur)) {
			Tcl_ListObjAppendElement(interp, list,
						 purple_tcl_ref_new(PurpleTclRefStatusType,
								  cur->data));
		}
		Tcl_SetObjResult(interp, list);
		break;
	case CMD_ACCOUNT_USERNAME:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "account");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj((char *)purple_account_get_username(account), -1));
		break;
	}
	return TCL_OK;
}
