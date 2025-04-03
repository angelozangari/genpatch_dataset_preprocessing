}
static PurpleBlistNode *tcl_list_to_buddy(Tcl_Interp *interp, int count, Tcl_Obj **elems)
{
	PurpleBlistNode *node = NULL;
	PurpleAccount *account;
	char *name;
	char *type;
	if (count < 3) {
		Tcl_SetObjResult(interp,
                                 Tcl_NewStringObj("list too short", -1));
		return NULL;
	}
	type = Tcl_GetString(elems[0]);
	name = Tcl_GetString(elems[1]);
	if ((account = tcl_validate_account(elems[2], interp)) == NULL)
		return NULL;
	if (!strcmp(type, "buddy")) {
		node = PURPLE_BLIST_NODE(purple_find_buddy(account, name));
	} else if (!strcmp(type, "group")) {
		node = PURPLE_BLIST_NODE(purple_blist_find_chat(account, name));
	}
	return node;
}
