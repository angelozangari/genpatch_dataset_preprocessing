}
static void purple_tcl_ref_dup(Tcl_Obj *obj1, Tcl_Obj *obj2)
{
	OBJ_REF_TYPE(obj2) = purple_stringref_ref(OBJ_REF_TYPE(obj1));
	OBJ_REF_VALUE(obj2) = OBJ_REF_VALUE(obj1);
}
