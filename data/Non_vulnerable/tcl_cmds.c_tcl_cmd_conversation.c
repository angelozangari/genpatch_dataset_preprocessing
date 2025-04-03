}
int tcl_cmd_conversation(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *list, *elem;
	const char *cmds[] = { "find", "handle", "list", "new", "write", "name", "title", "send", NULL };
	enum { CMD_CONV_FIND, CMD_CONV_HANDLE, CMD_CONV_LIST, CMD_CONV_NEW, CMD_CONV_WRITE , CMD_CONV_NAME, CMD_CONV_TITLE, CMD_CONV_SEND } cmd;
	const char *styles[] = { "send", "recv", "system", NULL };
	enum { CMD_CONV_WRITE_SEND, CMD_CONV_WRITE_RECV, CMD_CONV_WRITE_SYSTEM } style;
	const char *newopts[] = { "-chat", "-im" };
	enum { CMD_CONV_NEW_CHAT, CMD_CONV_NEW_IM } newopt;
	PurpleConversation *convo;
	PurpleAccount *account;
	PurpleConversationType type;
	GList *cur;
	char *opt, *from, *what;
	int error, argsused, flags = 0;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_CONV_FIND:
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "account name");
			return TCL_ERROR;
		}
		account = NULL;
		if ((account = tcl_validate_account(objv[2], interp)) == NULL)
			return TCL_ERROR;
		convo = purple_find_conversation_with_account(PURPLE_CONV_TYPE_ANY,
							    Tcl_GetString(objv[3]),
							    account);
		Tcl_SetObjResult(interp, purple_tcl_ref_new(PurpleTclRefConversation, convo));
		break;
	case CMD_CONV_HANDLE:
		if (objc != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "");
			return TCL_ERROR;
		}
		Tcl_SetObjResult(interp,
		                 purple_tcl_ref_new(PurpleTclRefHandle,
						    purple_conversations_get_handle()));
		break;
	case CMD_CONV_LIST:
		list = Tcl_NewListObj(0, NULL);
		for (cur = purple_get_conversations(); cur != NULL; cur = g_list_next(cur)) {
			elem = purple_tcl_ref_new(PurpleTclRefConversation, cur->data);
			Tcl_ListObjAppendElement(interp, list, elem);
		}
		Tcl_SetObjResult(interp, list);
		break;
	case CMD_CONV_NEW:
		if (objc < 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "?options? account name");
			return TCL_ERROR;
		}
		argsused = 2;
		type = PURPLE_CONV_TYPE_IM;
		while (argsused < objc) {
			opt = Tcl_GetString(objv[argsused]);
			if (*opt == '-') {
				if ((error = Tcl_GetIndexFromObj(interp, objv[argsused], newopts,
								 "option", 0, (int *)&newopt)) != TCL_OK)
					return error;
				argsused++;
				switch (newopt) {
				case CMD_CONV_NEW_CHAT:
					type = PURPLE_CONV_TYPE_CHAT;
					break;
				case CMD_CONV_NEW_IM:
					type = PURPLE_CONV_TYPE_IM;
					break;
				}
			} else {
				break;
			}
		}
		if (objc - argsused != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "?options? account name");
			return TCL_ERROR;
		}
		if ((account = tcl_validate_account(objv[argsused++], interp)) == NULL)
			return TCL_ERROR;
		convo = purple_conversation_new(type, account, Tcl_GetString(objv[argsused]));
		Tcl_SetObjResult(interp, purple_tcl_ref_new(PurpleTclRefConversation, convo));
		break;
	case CMD_CONV_WRITE:
		if (objc != 6) {
			Tcl_WrongNumArgs(interp, 2, objv, "conversation style from what");
			return TCL_ERROR;
		}
		if ((convo = tcl_validate_conversation(objv[2], interp)) == NULL)
			return TCL_ERROR;
		if ((error = Tcl_GetIndexFromObj(interp, objv[3], styles, "style", 0, (int *)&style)) != TCL_OK)
			return error;
		from = Tcl_GetString(objv[4]);
		what = Tcl_GetString(objv[5]);
		switch (style) {
		case CMD_CONV_WRITE_SEND:
			flags = PURPLE_MESSAGE_SEND;
			break;
		case CMD_CONV_WRITE_RECV:
			flags = PURPLE_MESSAGE_RECV;
			break;
		case CMD_CONV_WRITE_SYSTEM:
			flags = PURPLE_MESSAGE_SYSTEM;
			break;
		}
		if (purple_conversation_get_type(convo) == PURPLE_CONV_TYPE_CHAT)
			purple_conv_chat_write(PURPLE_CONV_CHAT(convo), from, what, flags, time(NULL));
		else
			purple_conv_im_write(PURPLE_CONV_IM(convo), from, what, flags, time(NULL));
		break;
	case CMD_CONV_NAME:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "conversation");
			return TCL_ERROR;
		}
		if ((convo = tcl_validate_conversation(objv[2], interp)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj((char *)purple_conversation_get_name(convo), -1));
		break;
	case CMD_CONV_TITLE:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "conversation");
			return TCL_ERROR;
		}
		if ((convo = tcl_validate_conversation(objv[2], interp)) == NULL)
			return TCL_ERROR;
		Tcl_SetObjResult(interp,
                                 Tcl_NewStringObj((char *)purple_conversation_get_title(convo), -1));
		break;
	case CMD_CONV_SEND:
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "conversation message");
			return TCL_ERROR;
		}
		if ((convo = tcl_validate_conversation(objv[2], interp)) == NULL)
			return TCL_ERROR;
		what = Tcl_GetString(objv[3]);
		if (purple_conversation_get_type(convo) == PURPLE_CONV_TYPE_CHAT)
			purple_conv_chat_send(PURPLE_CONV_CHAT(convo), what);
		else
			purple_conv_im_send(PURPLE_CONV_IM(convo), what);
		break;
	}
	return TCL_OK;
}
