}
static PurpleConversation *tcl_validate_conversation(Tcl_Obj *obj, Tcl_Interp *interp)
{
	PurpleConversation *convo;
	GList *cur;
	convo = purple_tcl_ref_get(interp, obj, PurpleTclRefConversation);
	if (convo == NULL)
		return NULL;
	for (cur = purple_get_conversations(); cur != NULL; cur = g_list_next(cur)) {
		if (convo == cur->data)
			return convo;
	}
	if (interp != NULL)
		Tcl_SetObjResult(interp, Tcl_NewStringObj("invalid conversation", -1));
	return NULL;
}
