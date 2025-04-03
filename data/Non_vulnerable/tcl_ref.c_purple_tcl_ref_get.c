}
void *purple_tcl_ref_get(Tcl_Interp *interp, Tcl_Obj *obj, PurpleStringref *type)
{
	if (obj->typePtr != &purple_tcl_ref) {
		if (Tcl_ConvertToType(interp, obj, &purple_tcl_ref) != TCL_OK)
			return NULL;
	}
	if (strcmp(purple_stringref_value(OBJ_REF_TYPE(obj)),
		   purple_stringref_value(type))) {
		if (interp) {
			Tcl_Obj *error = Tcl_NewStringObj("Bad Purple reference type: expected ", -1);
			Tcl_AppendToObj(error, purple_stringref_value(type), -1);
			Tcl_AppendToObj(error, " but got ", -1);
			Tcl_AppendToObj(error, purple_stringref_value(OBJ_REF_TYPE(obj)), -1);
			Tcl_SetObjResult(interp, error);
		}
		return NULL;
	}
	return OBJ_REF_VALUE(obj);
}
