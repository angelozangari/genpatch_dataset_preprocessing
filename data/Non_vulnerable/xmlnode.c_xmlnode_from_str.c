xmlnode *
xmlnode_from_str(const char *str, gssize size)
{
	struct _xmlnode_parser_data *xpd;
	xmlnode *ret;
	gsize real_size;
	g_return_val_if_fail(str != NULL, NULL);
	real_size = size < 0 ? strlen(str) : (gsize)size;
	xpd = g_new0(struct _xmlnode_parser_data, 1);
	if (xmlSAXUserParseMemory(&xmlnode_parser_libxml, xpd, str, real_size) < 0) {
		while(xpd->current && xpd->current->parent)
			xpd->current = xpd->current->parent;
		if(xpd->current)
			xmlnode_free(xpd->current);
		xpd->current = NULL;
	}
	ret = xpd->current;
	if (xpd->error) {
		ret = NULL;
		if (xpd->current)
			xmlnode_free(xpd->current);
	}
	g_free(xpd);
	return ret;
}
