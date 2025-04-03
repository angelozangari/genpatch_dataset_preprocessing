static void
xmlnode_parser_structural_error_libxml(void *user_data, xmlErrorPtr error)
{
	struct _xmlnode_parser_data *xpd = user_data;
	if (error && (error->level == XML_ERR_ERROR ||
	              error->level == XML_ERR_FATAL)) {
		xpd->error = TRUE;
		purple_debug_error("xmlnode", "XML parser error for xmlnode %p: "
		                   "Domain %i, code %i, level %i: %s",
		                   user_data, error->domain, error->code, error->level,
		                   error->message ? error->message : "(null)\n");
	} else if (error)
		purple_debug_warning("xmlnode", "XML parser error for xmlnode %p: "
		                     "Domain %i, code %i, level %i: %s",
		                     user_data, error->domain, error->code, error->level,
		                     error->message ? error->message : "(null)\n");
	else
		purple_debug_warning("xmlnode", "XML parser error for xmlnode %p\n",
		                     user_data);
}
