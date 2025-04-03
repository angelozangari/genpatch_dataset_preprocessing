}
static PurpleConnection *tcl_validate_gc(Tcl_Obj *obj, Tcl_Interp *interp)
{
	PurpleConnection *gc;
	GList *cur;
	gc = purple_tcl_ref_get(interp, obj, PurpleTclRefConnection);
	if (gc == NULL)
		return NULL;
	for (cur = purple_connections_get_all(); cur != NULL; cur = g_list_next(cur)) {
		if (gc == cur->data)
			return gc;
	}
	return NULL;
}
