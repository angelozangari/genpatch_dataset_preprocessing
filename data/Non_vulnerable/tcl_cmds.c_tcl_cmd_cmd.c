}
int tcl_cmd_cmd(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	const char *cmds[] = { "do", "help", "list", "register", "unregister", NULL };
	enum { CMD_CMD_DO, CMD_CMD_HELP, CMD_CMD_LIST, CMD_CMD_REGISTER, CMD_CMD_UNREGISTER } cmd;
	struct tcl_cmd_handler *handler;
	Tcl_Obj *list, *elem;
	PurpleConversation *convo;
	PurpleCmdId id;
	PurpleCmdStatus status;
	int error;
	GList *l, *cur;
	gchar *escaped, *errstr = NULL;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_CMD_DO:
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "conversation command");
			return TCL_ERROR;
		}
		if ((convo = tcl_validate_conversation(objv[2], interp)) == NULL)
			return TCL_ERROR;
		escaped = g_markup_escape_text(Tcl_GetString(objv[3]), -1);
		status = purple_cmd_do_command(convo, Tcl_GetString(objv[3]),
				escaped, &errstr);
		g_free(escaped);
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj(errstr ? (char *)errstr : "", -1));
		g_free(errstr);
		if (status != PURPLE_CMD_STATUS_OK) {
			return TCL_ERROR;
		}
		break;
	case CMD_CMD_HELP:
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "conversation name");
			return TCL_ERROR;
		}
		if ((convo = tcl_validate_conversation(objv[2], interp)) == NULL)
			return TCL_ERROR;
		l = cur = purple_cmd_help(convo, Tcl_GetString(objv[3]));
		list = Tcl_NewListObj(0, NULL);
		while (cur != NULL) {
			elem = Tcl_NewStringObj((char *)cur->data, -1);
			Tcl_ListObjAppendElement(interp, list, elem);
			cur = g_list_next(cur);
		}
		g_list_free(l);
		Tcl_SetObjResult(interp, list);
		break;
	case CMD_CMD_LIST:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "conversation");
			return TCL_ERROR;
		}
		if ((convo = tcl_validate_conversation(objv[2], interp)) == NULL)
			return TCL_ERROR;
		l = cur = purple_cmd_list(convo);
		list = Tcl_NewListObj(0, NULL);
		while (cur != NULL) {
			elem = Tcl_NewStringObj((char *)cur->data, -1);
			Tcl_ListObjAppendElement(interp, list, elem);
			cur = g_list_next(cur);
		}
		g_list_free(l);
		Tcl_SetObjResult(interp, list);
		break;
	case CMD_CMD_REGISTER:
		if (objc != 9) {
			Tcl_WrongNumArgs(interp, 2, objv, "cmd arglist priority flags prpl_id proc helpstr");
			return TCL_ERROR;
		}
		handler = g_new0(struct tcl_cmd_handler, 1);
		handler->cmd = objv[2];
		handler->args = Tcl_GetString(objv[3]);
		handler->nargs = strlen(handler->args);
		if ((error = Tcl_GetIntFromObj(interp, objv[4],
		                               &handler->priority)) != TCL_OK) {
			g_free(handler);
			return error;
		}
		if ((error = Tcl_GetIntFromObj(interp, objv[5],
		                               &handler->flags)) != TCL_OK) {
			g_free(handler);
			return error;
		}
		handler->prpl_id = Tcl_GetString(objv[6]);
		handler->proc = objv[7];
		handler->helpstr = Tcl_GetString(objv[8]);
		handler->interp = interp;
		if ((id = tcl_cmd_register(handler)) == 0) {
			tcl_cmd_handler_free(handler);
			Tcl_SetObjResult(interp, Tcl_NewIntObj(0));
		} else {
			handler->id = id;
			Tcl_SetObjResult(interp, Tcl_NewIntObj(id));
		}
		break;
	case CMD_CMD_UNREGISTER:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "id");
			return TCL_ERROR;
		}
		if ((error = Tcl_GetIntFromObj(interp, objv[2],
		                               (int *)&id)) != TCL_OK)
			return error;
		tcl_cmd_unregister(id, interp);
		break;
	}
	return TCL_OK;
}
