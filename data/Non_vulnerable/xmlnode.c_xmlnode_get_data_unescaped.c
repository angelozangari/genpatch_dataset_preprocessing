char *
xmlnode_get_data_unescaped(const xmlnode *node)
{
	char *escaped = xmlnode_get_data(node);
	char *unescaped = escaped ? purple_unescape_html(escaped) : NULL;
	g_free(escaped);
	return unescaped;
}
