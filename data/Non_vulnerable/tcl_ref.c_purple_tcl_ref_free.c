}
static void purple_tcl_ref_free(Tcl_Obj *obj)
{
	purple_stringref_unref(OBJ_REF_TYPE(obj));
}
