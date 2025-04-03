static PurpleConnection *tcl_validate_gc(Tcl_Obj *obj, Tcl_Interp *interp);
static PurpleAccount *tcl_validate_account(Tcl_Obj *obj, Tcl_Interp *interp)
{
	PurpleAccount *account;
	GList *cur;
	account = purple_tcl_ref_get(interp, obj, PurpleTclRefAccount);
	if (account == NULL)
		return NULL;
	for (cur = purple_accounts_get_all(); cur != NULL; cur = g_list_next(cur)) {
		if (account == cur->data)
			return account;
	}
	if (interp != NULL)
		Tcl_SetObjResult(interp, Tcl_NewStringObj("invalid account", -1));
	return NULL;
}
