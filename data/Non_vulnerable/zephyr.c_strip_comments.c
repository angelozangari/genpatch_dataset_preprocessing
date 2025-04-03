}
static void strip_comments(char *str)
{
	char *tmp = strchr(str, '#');
	if (tmp)
		*tmp = '\0';
	g_strchug(str);
	g_strchomp(str);
}
