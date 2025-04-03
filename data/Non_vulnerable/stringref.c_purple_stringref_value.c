}
const char *purple_stringref_value(const PurpleStringref *stringref)
{
	return (stringref == NULL ? NULL : stringref->value);
}
