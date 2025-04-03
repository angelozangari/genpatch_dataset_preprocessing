}
int purple_stringref_cmp(const PurpleStringref *s1, const PurpleStringref *s2)
{
	return (s1 == s2 ? 0 : strcmp(purple_stringref_value(s1), purple_stringref_value(s2)));
}
