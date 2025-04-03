static void
xmlnode_parser_element_end_libxml(void *user_data, const xmlChar *element_name,
				 const xmlChar *prefix, const xmlChar *xmlns)
{
	struct _xmlnode_parser_data *xpd = user_data;
	if(!element_name || !xpd->current || xpd->error)
		return;
	if(xpd->current->parent) {
		if(!xmlStrcmp((xmlChar*) xpd->current->name, element_name))
			xpd->current = xpd->current->parent;
	}
}
