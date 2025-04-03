}
PurpleStringref *purple_stringref_new_noref(const char *value)
{
	PurpleStringref *newref;
	if (value == NULL)
		return NULL;
	newref = g_malloc(sizeof(PurpleStringref) + strlen(value));
	strcpy(newref->value, value);
	newref->ref = 0x80000000;
	if (gclist == NULL)
		purple_timeout_add(0, gs_idle_cb, NULL);
	gclist = g_list_prepend(gclist, newref);
	return newref;
}
