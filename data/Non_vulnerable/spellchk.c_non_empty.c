}
static gboolean non_empty(const char *s)
{
	while (*s && g_ascii_isspace(*s))
		s++;
	return *s;
}
