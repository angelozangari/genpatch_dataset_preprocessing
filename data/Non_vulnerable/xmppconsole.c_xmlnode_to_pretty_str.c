static char *
xmlnode_to_pretty_str(xmlnode *node, int *len, int depth)
{
	GString *text = g_string_new("");
	xmlnode *c;
	char *node_name, *esc, *esc2, *tab = NULL;
	gboolean need_end = FALSE, pretty = TRUE;
	g_return_val_if_fail(node != NULL, NULL);
	if (pretty && depth) {
		tab = g_strnfill(depth, '\t');
		text = g_string_append(text, tab);
	}
	node_name = g_markup_escape_text(node->name, -1);
	g_string_append_printf(text,
	                       "<font color='" BRACKET_COLOR "'>&lt;</font>"
	                       "<font color='" TAG_COLOR "'><b>%s</b></font>",
	                       node_name);
	if (node->xmlns) {
		if ((!node->parent ||
		     !node->parent->xmlns ||
		     strcmp(node->xmlns, node->parent->xmlns)) &&
		    strcmp(node->xmlns, "jabber:client"))
		{
			char *xmlns = g_markup_escape_text(node->xmlns, -1);
			g_string_append_printf(text,
			                       " <font color='" ATTR_NAME_COLOR "'><b>xmlns</b></font>="
			                       "'<font color='" XMLNS_COLOR "'><b>%s</b></font>'",
			                       xmlns);
			g_free(xmlns);
		}
	}
	for (c = node->child; c; c = c->next)
	{
		if (c->type == XMLNODE_TYPE_ATTRIB) {
			esc = g_markup_escape_text(c->name, -1);
			esc2 = g_markup_escape_text(c->data, -1);
			g_string_append_printf(text,
			                       " <font color='" ATTR_NAME_COLOR "'><b>%s</b></font>="
			                       "'<font color='" ATTR_VALUE_COLOR "'>%s</font>'",
			                       esc, esc2);
			g_free(esc);
			g_free(esc2);
		} else if (c->type == XMLNODE_TYPE_TAG || c->type == XMLNODE_TYPE_DATA) {
			if (c->type == XMLNODE_TYPE_DATA)
				pretty = FALSE;
			need_end = TRUE;
		}
	}
	if (need_end) {
		g_string_append_printf(text,
		                       "<font color='"BRACKET_COLOR"'>&gt;</font>%s",
		                       pretty ? "<br>" : "");
		for (c = node->child; c; c = c->next)
		{
			if (c->type == XMLNODE_TYPE_TAG) {
				int esc_len;
				esc = xmlnode_to_pretty_str(c, &esc_len, depth+1);
				text = g_string_append_len(text, esc, esc_len);
				g_free(esc);
			} else if (c->type == XMLNODE_TYPE_DATA && c->data_sz > 0) {
				esc = g_markup_escape_text(c->data, c->data_sz);
				text = g_string_append(text, esc);
				g_free(esc);
			}
		}
		if(tab && pretty)
			text = g_string_append(text, tab);
		g_string_append_printf(text,
		                       "<font color='" BRACKET_COLOR "'>&lt;</font>/"
		                       "<font color='" TAG_COLOR "'><b>%s</b></font>"
		                       "<font color='" BRACKET_COLOR "'>&gt;</font><br>",
		                       node_name);
	} else {
		g_string_append_printf(text,
		                       "/<font color='" BRACKET_COLOR "'>&gt;</font><br>");
	}
	g_free(node_name);
	g_free(tab);
	if(len)
		*len = text->len;
	return g_string_free(text, FALSE);
}
