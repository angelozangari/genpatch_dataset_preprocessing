char *
xmlnode_to_formatted_str(const xmlnode *node, int *len)
{
	char *xml, *xml_with_declaration;
	g_return_val_if_fail(node != NULL, NULL);
	xml = xmlnode_to_str_helper(node, len, TRUE, 0);
	xml_with_declaration =
		g_strdup_printf("<?xml version='1.0' encoding='UTF-8' ?>" NEWLINE_S NEWLINE_S "%s", xml);
	g_free(xml);
	if (len)
		*len += sizeof("<?xml version='1.0' encoding='UTF-8' ?>" NEWLINE_S NEWLINE_S) - 1;
	return xml_with_declaration;
}
