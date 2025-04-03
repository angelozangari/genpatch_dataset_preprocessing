static void
xmlnode_parser_element_start_libxml(void *user_data,
				   const xmlChar *element_name, const xmlChar *prefix, const xmlChar *xmlns,
				   int nb_namespaces, const xmlChar **namespaces,
				   int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
	struct _xmlnode_parser_data *xpd = user_data;
	xmlnode *node;
	int i, j;
	if(!element_name || xpd->error) {
		return;
	} else {
		if(xpd->current)
			node = xmlnode_new_child(xpd->current, (const char*) element_name);
		else
			node = xmlnode_new((const char *) element_name);
		xmlnode_set_namespace(node, (const char *) xmlns);
		xmlnode_set_prefix(node, (const char *)prefix);
		if (nb_namespaces != 0) {
			node->namespace_map = g_hash_table_new_full(
				g_str_hash, g_str_equal, g_free, g_free);
			for (i = 0, j = 0; i < nb_namespaces; i++, j += 2) {
				const char *key = (const char *)namespaces[j];
				const char *val = (const char *)namespaces[j + 1];
				g_hash_table_insert(node->namespace_map,
					g_strdup(key ? key : ""), g_strdup(val ? val : ""));
			}
		}
		for(i=0; i < nb_attributes * 5; i+=5) {
			const char *name = (const char *)attributes[i];
			const char *prefix = (const char *)attributes[i+1];
			char *txt;
			int attrib_len = attributes[i+4] - attributes[i+3];
			char *attrib = g_strndup((const char *)attributes[i+3], attrib_len);
			txt = attrib;
			attrib = purple_unescape_text(txt);
			g_free(txt);
			xmlnode_set_attrib_full(node, name, NULL, prefix, attrib);
			g_free(attrib);
		}
		xpd->current = node;
	}
}
