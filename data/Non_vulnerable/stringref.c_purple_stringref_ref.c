}
PurpleStringref *purple_stringref_ref(PurpleStringref *stringref)
{
	if (stringref == NULL)
		return NULL;
	stringref->ref++;
	return stringref;
}
