}
static void stringref_free(PurpleStringref *stringref)
{
#ifdef DEBUG
	if (REFCOUNT(stringref->ref) != 0) {
		purple_debug(PURPLE_DEBUG_ERROR, "stringref", "Free of nonzero (%d) ref stringref!\n", REFCOUNT(stringref->ref));
		return;
	}
#endif /* DEBUG */
	g_free(stringref);
}
