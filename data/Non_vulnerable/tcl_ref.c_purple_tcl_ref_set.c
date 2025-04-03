 * really rather dup it.  Oh, well. */
static int purple_tcl_ref_set(Tcl_Interp *interp, Tcl_Obj *obj)
{
	char *bytes = Tcl_GetStringFromObj(obj, NULL);
	char *ptr;
	PurpleStringref *type;
	void *value;
	static const char prefix[] = "purple-";
	static const gsize prefixlen = sizeof(prefix) - 1;
	if (strlen(bytes) < prefixlen
	    || strncmp(bytes, prefix, prefixlen)
	    || (ptr = strchr(bytes, ':')) == NULL
	    || (gsize)(ptr - bytes) == prefixlen)
		goto badobject;
	/* Bad Ethan */
	*ptr = '\0';
	type = purple_stringref_new(bytes + prefixlen);
	*ptr = ':';
	ptr++;
	if (sscanf(ptr, "%p", &value) == 0) {
		purple_stringref_unref(type);
		goto badobject;
	}
	/* At this point we know we have a good object; free the old and
	 * install our internal representation. */
	if (obj->typePtr != NULL && obj->typePtr->freeIntRepProc != NULL)
		obj->typePtr->freeIntRepProc(obj);
	obj->typePtr = &purple_tcl_ref;
	OBJ_REF_TYPE(obj) = type;
	OBJ_REF_VALUE(obj) = value;
	return TCL_OK;
badobject:
	if (interp) {
		Tcl_SetObjResult(interp,
				 Tcl_NewStringObj("invalid PurpleTclRef representation", -1));
	}
	return TCL_ERROR;
}
