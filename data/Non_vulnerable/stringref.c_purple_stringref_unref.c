}
void purple_stringref_unref(PurpleStringref *stringref)
{
	if (stringref == NULL)
		return;
	if (REFCOUNT(--(stringref->ref)) == 0) {
		if (stringref->ref & 0x80000000)
			gclist = g_list_remove(gclist, stringref);
		stringref_free(stringref);
	}
}
