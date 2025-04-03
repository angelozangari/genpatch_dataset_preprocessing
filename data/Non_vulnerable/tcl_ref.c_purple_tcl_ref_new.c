}
Tcl_Obj *purple_tcl_ref_new(PurpleStringref *type, void *value)
{
	Tcl_Obj *obj = Tcl_NewObj();
	obj->typePtr = &purple_tcl_ref;
	OBJ_REF_TYPE(obj) = purple_stringref_ref(type);
	OBJ_REF_VALUE(obj) = value;
	Tcl_InvalidateStringRep(obj);
	return obj;
}
