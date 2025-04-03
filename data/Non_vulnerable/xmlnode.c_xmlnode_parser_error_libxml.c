static void
xmlnode_parser_error_libxml(void *user_data, const char *msg, ...)
{
	struct _xmlnode_parser_data *xpd = user_data;
	char errmsg[2048];
	va_list args;
	xpd->error = TRUE;
	va_start(args, msg);
	vsnprintf(errmsg, sizeof(errmsg), msg, args);
	va_end(args);
	purple_debug_error("xmlnode", "Error parsing xml file: %s", errmsg);
}
