}
int tcl_cmd_buddy(ClientData unused, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *list, *tclgroup, *tclgrouplist, *tclcontact, *tclcontactlist, *tclbud, **elems, *result;
	const char *cmds[] = { "alias", "handle", "info", "list", NULL };
	enum { CMD_BUDDY_ALIAS, CMD_BUDDY_HANDLE, CMD_BUDDY_INFO, CMD_BUDDY_LIST } cmd;
	PurpleBlistNodeType type;
	PurpleBlistNode *node, *gnode, *bnode;
	PurpleAccount *account;
	PurpleBuddy *bud;
	PurpleChat *cnode;
	int error, all = 0, count;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?args?");
		return TCL_ERROR;
	}
	if ((error = Tcl_GetIndexFromObj(interp, objv[1], cmds, "subcommand", 0, (int *)&cmd)) != TCL_OK)
		return error;
	switch (cmd) {
	case CMD_BUDDY_ALIAS:
		if (objc != 3) {
			Tcl_WrongNumArgs(interp, 2, objv, "buddy");
			return TCL_ERROR;
		}
		if ((error = Tcl_ListObjGetElements(interp, objv[2], &count, &elems)) != TCL_OK)
			return error;
		if ((node = tcl_list_to_buddy(interp, count, elems)) == NULL)
			return TCL_ERROR;
		type = purple_blist_node_get_type(node);
		if (type == PURPLE_BLIST_CHAT_NODE)
			Tcl_SetObjResult(interp,
					 Tcl_NewStringObj(purple_chat_get_name((PurpleChat *)node), -1));
		else if (type == PURPLE_BLIST_BUDDY_NODE)
			Tcl_SetObjResult(interp,
                                         Tcl_NewStringObj((char *)purple_buddy_get_alias((PurpleBuddy *)node), -1));
		return TCL_OK;
		break;
	case CMD_BUDDY_HANDLE:
		if (objc != 2) {
			Tcl_WrongNumArgs(interp, 2, objv, "");
			return TCL_ERROR;
		}
		Tcl_SetObjResult(interp,
				 purple_tcl_ref_new(PurpleTclRefHandle,
						    purple_blist_get_handle()));
		break;
	case CMD_BUDDY_INFO:
		if (objc != 3 && objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "( buddy | account username )");
			return TCL_ERROR;
		}
		if (objc == 3) {
			if ((error = Tcl_ListObjGetElements(interp, objv[2], &count, &elems)) != TCL_OK)
				return error;
			if (count < 3) {
				Tcl_SetObjResult(interp,
						 Tcl_NewStringObj("buddy too short", -1));
				return TCL_ERROR;
			}
			if (strcmp("buddy", Tcl_GetString(elems[0]))) {
				Tcl_SetObjResult(interp,
						 Tcl_NewStringObj("invalid buddy", -1));
				return TCL_ERROR;
			}
			if ((account = tcl_validate_account(elems[2], interp)) == NULL)
				return TCL_ERROR;
			serv_get_info(purple_account_get_connection(account), Tcl_GetString(elems[1]));
		} else {
			if ((account = tcl_validate_account(objv[2], interp)) == NULL)
				return TCL_ERROR;
			serv_get_info(purple_account_get_connection(account), Tcl_GetString(objv[3]));
		}
		break;
	case CMD_BUDDY_LIST:
		if (objc == 3) {
			if (!strcmp("-all", Tcl_GetString(objv[2]))) {
				all = 1;
			} else {
				result = Tcl_NewStringObj("",-1);
				Tcl_AppendStringsToObj(result, "unknown option: ", Tcl_GetString(objv[2]), NULL);
				Tcl_SetObjResult(interp,result);
				return TCL_ERROR;
			}
		}
		list = Tcl_NewListObj(0, NULL);
		for (gnode = purple_blist_get_root(); gnode != NULL; gnode = purple_blist_node_get_sibling_next(gnode)) {
			tclgroup = Tcl_NewListObj(0, NULL);
			Tcl_ListObjAppendElement(interp, tclgroup, Tcl_NewStringObj("group", -1));
			Tcl_ListObjAppendElement(interp, tclgroup,
						 Tcl_NewStringObj(purple_group_get_name((PurpleGroup *)gnode), -1));
			tclgrouplist = Tcl_NewListObj(0, NULL);
			for (node = purple_blist_node_get_first_child(gnode); node != NULL; node = purple_blist_node_get_sibling_next(node)) {
				PurpleAccount *account;
				type = purple_blist_node_get_type(node);
				switch (type) {
				case PURPLE_BLIST_CONTACT_NODE:
					tclcontact = Tcl_NewListObj(0, NULL);
					Tcl_IncrRefCount(tclcontact);
					Tcl_ListObjAppendElement(interp, tclcontact, Tcl_NewStringObj("contact", -1));
					tclcontactlist = Tcl_NewListObj(0, NULL);
					Tcl_IncrRefCount(tclcontactlist);
					count = 0;
					for (bnode = purple_blist_node_get_first_child(node); bnode != NULL; bnode = purple_blist_node_get_sibling_next(bnode)) {
						if (purple_blist_node_get_type(bnode) != PURPLE_BLIST_BUDDY_NODE)
							continue;
						bud = (PurpleBuddy *)bnode;
						account = purple_buddy_get_account(bud);
						if (!all && !purple_account_is_connected(account))
							continue;
						count++;
						tclbud = Tcl_NewListObj(0, NULL);
						Tcl_ListObjAppendElement(interp, tclbud, Tcl_NewStringObj("buddy", -1));
						Tcl_ListObjAppendElement(interp, tclbud, Tcl_NewStringObj(purple_buddy_get_name(bud), -1));
						Tcl_ListObjAppendElement(interp, tclbud, purple_tcl_ref_new(PurpleTclRefAccount, account));
						Tcl_ListObjAppendElement(interp, tclcontactlist, tclbud);
					}
					if (count) {
						Tcl_ListObjAppendElement(interp, tclcontact, tclcontactlist);
						Tcl_ListObjAppendElement(interp, tclgrouplist, tclcontact);
					}
					Tcl_DecrRefCount(tclcontact);
					Tcl_DecrRefCount(tclcontactlist);
					break;
				case PURPLE_BLIST_CHAT_NODE:
					cnode = (PurpleChat *)node;
					account = purple_chat_get_account(cnode);
					if (!all && !purple_account_is_connected(account))
						continue;
					tclbud = Tcl_NewListObj(0, NULL);
					Tcl_ListObjAppendElement(interp, tclbud, Tcl_NewStringObj("chat", -1));
					Tcl_ListObjAppendElement(interp, tclbud, Tcl_NewStringObj(purple_chat_get_name(cnode), -1));
					Tcl_ListObjAppendElement(interp, tclbud, purple_tcl_ref_new(PurpleTclRefAccount, account));
					Tcl_ListObjAppendElement(interp, tclgrouplist, tclbud);
					break;
				default:
					purple_debug(PURPLE_DEBUG_WARNING, "tcl", "Unexpected buddy type %d", type);
					continue;
				}
			}
			Tcl_ListObjAppendElement(interp, tclgroup, tclgrouplist);
			Tcl_ListObjAppendElement(interp, list, tclgroup);
		}
		Tcl_SetObjResult(interp, list);
		break;
	}
	return TCL_OK;
}
