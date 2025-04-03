static int
Z_AddField(char **ptr, const char *field, char *end)
{
    register int len;
    len = field ? strlen (field) + 1 : 1;
    if (*ptr+len > end)
	return 1;
    if (field)
        strcpy(*ptr, field);
    else
      **ptr = '\0';
    *ptr += len;
    return 0;
}
