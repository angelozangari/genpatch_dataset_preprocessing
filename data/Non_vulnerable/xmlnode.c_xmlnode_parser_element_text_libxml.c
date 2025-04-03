static void
xmlnode_parser_element_text_libxml(void *user_data, const xmlChar *text, int text_len)
{
	struct _xmlnode_parser_data *xpd = user_data;
	if(!xpd->current || xpd->error)
		return;
	if(!text || !text_len)
		return;
	xmlnode_insert_data(xpd->current, (const char*) text, text_len);
}
