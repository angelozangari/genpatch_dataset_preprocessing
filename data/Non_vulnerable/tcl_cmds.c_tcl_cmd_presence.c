}
int tcl_cmd_presence(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	const char *cmds[] = { "account", "active_status", "available",
			       "chat_user", "context", "conversation", "idle",
			       "login", "online", "status", "statuses", NULL };
	enum { CMD_PRESENCE_ACCOUNT, CMD_PRESENCE_ACTIVE_STATUS,
	       CMD_PRESENCE_AVAILABLE, CMD_PRESENCE_CHAT_USER,
	       CMD_PRESENCE_CONTEXT, CMD_PRESENCE_CONVERSATION,
	       CMD_PRESENCE_IDLE, CMD_PRESENCE_LOGIN, CMD_PRESENCE_ONLINE,
	       CMD_PRESENCE_STATUS, CMD_PRESENCE_STATUSES } cmd;
	Tcl_Obj *result;
	Tcl_Obj *list, *elem;
	PurplePresence *presence;
	GList *cur;
	int error, idle, idle_time, login_time;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_PRESENCE_ACCOUNT:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp, purple_tcl_ref_new(PurpleTclRefAccount,
		                                          purple_presence_get_account(presence)));
		break;
	case CMD_PRESENCE_ACTIVE_STATUS:
		if (objc != 3 && objc != 4 && objc != 5) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence [?status_id? | ?-primitive primitive?]");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		if (objc == 3) {
			Tcl_SetObjResult(interp,
					 purple_tcl_ref_new(PurpleTclRefStatus,
							  purple_presence_get_active_status(presence)));
		} else if (objc == 4) {
			Tcl_SetObjResult(interp,
                                         Tcl_NewBooleanObj(
						 purple_presence_is_status_active(presence,
										  Tcl_GetString(objv[3]))));
		} else {
			PurpleStatusPrimitive primitive;
			if (strcmp(Tcl_GetString(objv[3]), "-primitive")) {
				result = Tcl_NewStringObj("bad option \"", -1);
				Tcl_AppendObjToObj(result, objv[3]);
				Tcl_AppendToObj(result,
						"\": should be -primitive", -1);
				Tcl_SetObjResult(interp,result);
				return TCL_ERROR;
			}
			primitive = purple_primitive_get_type_from_id(Tcl_GetString(objv[4]));
			if (primitive == PURPLE_STATUS_UNSET) {
				result = Tcl_NewStringObj("invalid primitive ", -1);
				Tcl_AppendObjToObj(result, objv[4]);
				Tcl_SetObjResult(interp,result);
				return TCL_ERROR;
			}
			Tcl_SetObjResult(interp,
                                         Tcl_NewBooleanObj(
						 purple_presence_is_status_primitive_active(presence, primitive)));
			break;
		}
		break;
	case CMD_PRESENCE_AVAILABLE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewBooleanObj(purple_presence_is_available(presence)));
		break;
	case CMD_PRESENCE_CHAT_USER:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj(purple_presence_get_chat_user(presence), -1));
		break;
	case CMD_PRESENCE_CONTEXT:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		switch (purple_presence_get_context(presence)) {
		case PURPLE_PRESENCE_CONTEXT_UNSET:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("unset", -1));
			break;
		case PURPLE_PRESENCE_CONTEXT_ACCOUNT:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("account", -1));
			break;
		case PURPLE_PRESENCE_CONTEXT_CONV:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("conversation", -1));
			break;
		case PURPLE_PRESENCE_CONTEXT_BUDDY:
			Tcl_SetObjResult(interp, Tcl_NewStringObj("buddy", -1));
			break;
		}
		break;
	case CMD_PRESENCE_CONVERSATION:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp, purple_tcl_ref_new(PurpleTclRefConversation,
		                                          purple_presence_get_conversation(presence)));
		break;
	case CMD_PRESENCE_IDLE:
		if (objc < 3 || objc > 5) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence ?idle? ?time?");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		if (objc == 3) {
			if (purple_presence_is_idle(presence)) {
				idle_time = purple_presence_get_idle_time (presence);
				Tcl_SetObjResult(interp, Tcl_NewIntObj(idle_time));
			} else {
				result = Tcl_NewListObj(0, NULL);
				Tcl_SetObjResult(interp, result);
			}
			break;
		}
		if ((error = Tcl_GetBooleanFromObj(interp, objv[3], &idle)) != TCL_OK)
			return TCL_ERROR;
		if (objc == 4) {
			purple_presence_set_idle(presence, idle, time(NULL));
		} else if (objc == 5) {
			if ((error = Tcl_GetIntFromObj(interp,
		                                       objv[4],
		                                       &idle_time)) != TCL_OK)
				return TCL_ERROR;
			purple_presence_set_idle(presence, idle, idle_time);
		}
		break;
	case CMD_PRESENCE_LOGIN:
		if (objc != 3 && objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence ?time?");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		if (objc == 3) {
			Tcl_SetObjResult(interp, Tcl_NewIntObj(purple_presence_get_login_time(presence)));
		} else {
			if ((error == Tcl_GetIntFromObj(interp,
			                                objv[3],
			                                &login_time)) != TCL_OK)
				return TCL_ERROR;
			purple_presence_set_login_time(presence, login_time);
		}
		break;
	case CMD_PRESENCE_ONLINE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewBooleanObj(
					 purple_presence_is_online(presence)));
		break;
	case CMD_PRESENCE_STATUS:
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence status_id");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
		                 purple_tcl_ref_new(PurpleTclRefStatus,
		                                  purple_presence_get_status(presence,
		                                                           Tcl_GetString(objv[3]))));
		break;
	case CMD_PRESENCE_STATUSES:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "presence");
			return TCL_ERROR;
		}
		if ((presence = purple_tcl_ref_get(interp, objv[2], PurpleTclRefPresence)) == NULL)
			return TCL_ERROR;
		list = Tcl_NewListObj(0, NULL);
		for (cur = purple_presence_get_statuses(presence); cur != NULL;
		     cur = g_list_next(cur)) {
			elem = purple_tcl_ref_new(PurpleTclRefStatus, cur->data);
			Tcl_ListObjAppendElement(interp, list, elem);
		}
		Tcl_SetObjResult(interp, list);
		break;
	}
	return TCL_OK;
}
