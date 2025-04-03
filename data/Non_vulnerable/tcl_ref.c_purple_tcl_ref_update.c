}
static void purple_tcl_ref_update(Tcl_Obj *obj)
{
	size_t len;
	/* This is ugly on memory, but we pretty much have to either
	 * do this or guesstimate lengths or introduce a varargs
	 * function in here ... ugh. */
	char *bytes = g_strdup_printf("purple-%s:%p",
				      purple_stringref_value(OBJ_REF_TYPE(obj)),
				      OBJ_REF_VALUE(obj));
	obj->length = strlen(bytes);
	len = obj->length + 1;
	obj->bytes = ckalloc(len);
	g_strlcpy(obj->bytes, bytes, len);
	g_free(bytes);
}
