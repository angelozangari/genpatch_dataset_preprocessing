}
size_t purple_stringref_len(const PurpleStringref *stringref)
{
	return strlen(purple_stringref_value(stringref));
}
