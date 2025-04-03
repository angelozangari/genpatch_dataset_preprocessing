static gboolean gs_idle_cb(gpointer data);
PurpleStringref *purple_stringref_new(const char *value)
{
	PurpleStringref *newref;
	size_t len;
	if (value == NULL)
		return NULL;
	len = strlen(value);
	newref = g_malloc(sizeof(PurpleStringref) + len);
	/* g_strlcpy() takes the size of the buffer, including the NUL.
	   strlen() returns the length of the string, without the NUL. */
	g_strlcpy(newref->value, value, len + 1);
	newref->ref = 1;
	return newref;
}
