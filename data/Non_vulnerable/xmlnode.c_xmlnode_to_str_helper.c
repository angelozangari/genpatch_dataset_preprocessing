static char *
xmlnode_to_str_helper(const xmlnode *node, int *len, gboolean formatting, int depth)
{
	GString *text = g_string_new("");
	const char *prefix;
	const xmlnode *c;
	char *node_name, *esc, *esc2, *tab = NULL;
	gboolean need_end = FALSE, pretty = formatting;
	g_return_val_if_fail(node != NULL, NULL);
	if(pretty && depth) {
		tab = g_strnfill(depth, '\t');
		text = g_string_append(text, tab);
	}
	node_name = g_markup_escape_text(node->name, -1);
	prefix = xmlnode_get_prefix(node);
	if (prefix) {
		g_string_append_printf(text, "<%s:%s", prefix, node_name);
	} else {
		g_string_append_printf(text, "<%s", node_name);
	}
	if (node->namespace_map) {
		g_hash_table_foreach(node->namespace_map,
			(GHFunc)xmlnode_to_str_foreach_append_ns, text);
	} else if (node->xmlns) {
		if(!node->parent || !purple_strequal(node->xmlns, node->parent->xmlns))
		{
			char *xmlns = g_markup_escape_text(node->xmlns, -1);
			g_string_append_printf(text, " xmlns='%s'", xmlns);
			g_free(xmlns);
		}
	}
	for(c = node->child; c; c = c->next)
	{
		if(c->type == XMLNODE_TYPE_ATTRIB) {
			const char *aprefix = xmlnode_get_prefix(c);
			esc = g_markup_escape_text(c->name, -1);
			esc2 = g_markup_escape_text(c->data, -1);
			if (aprefix) {
				g_string_append_printf(text, " %s:%s='%s'", aprefix, esc, esc2);
			} else {
				g_string_append_printf(text, " %s='%s'", esc, esc2);
			}
			g_free(esc);
			g_free(esc2);
		} else if(c->type == XMLNODE_TYPE_TAG || c->type == XMLNODE_TYPE_DATA) {
			if(c->type == XMLNODE_TYPE_DATA)
				pretty = FALSE;
			need_end = TRUE;
		}
	}
	if(need_end) {
		g_string_append_printf(text, ">%s", pretty ? NEWLINE_S : "");
		for(c = node->child; c; c = c->next)
		{
			if(c->type == XMLNODE_TYPE_TAG) {
				int esc_len;
				esc = xmlnode_to_str_helper(c, &esc_len, pretty, depth+1);
				text = g_string_append_len(text, esc, esc_len);
				g_free(esc);
			} else if(c->type == XMLNODE_TYPE_DATA && c->data_sz > 0) {
				esc = g_markup_escape_text(c->data, c->data_sz);
				text = g_string_append(text, esc);
				g_free(esc);
			}
		}
		if(tab && pretty)
			text = g_string_append(text, tab);
		if (prefix) {
			g_string_append_printf(text, "</%s:%s>%s", prefix, node_name, formatting ? NEWLINE_S : "");
		} else {
			g_string_append_printf(text, "</%s>%s", node_name, formatting ? NEWLINE_S : "");
		}
	} else {
		g_string_append_printf(text, "/>%s", formatting ? NEWLINE_S : "");
	}
	g_free(node_name);
	g_free(tab);
	if(len)
		*len = text->len;
	return g_string_free(text, FALSE);
}
