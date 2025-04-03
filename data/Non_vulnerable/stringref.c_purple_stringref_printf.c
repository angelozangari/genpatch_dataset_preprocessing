}
PurpleStringref *purple_stringref_printf(const char *format, ...)
{
	PurpleStringref *newref;
	va_list ap;
	if (format == NULL)
		return NULL;
	va_start(ap, format);
	newref = g_malloc(sizeof(PurpleStringref) + g_printf_string_upper_bound(format, ap));
	vsprintf(newref->value, format, ap);
	va_end(ap);
	newref->ref = 1;
	return newref;
}
