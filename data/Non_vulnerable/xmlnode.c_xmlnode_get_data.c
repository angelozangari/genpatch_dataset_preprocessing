char *
xmlnode_get_data(const xmlnode *node)
{
	GString *str = NULL;
	xmlnode *c;
	g_return_val_if_fail(node != NULL, NULL);
	for(c = node->child; c; c = c->next) {
		if(c->type == XMLNODE_TYPE_DATA) {
			if(!str)
				str = g_string_new_len(c->data, c->data_sz);
			else
				str = g_string_append_len(str, c->data, c->data_sz);
		}
	}
	if (str == NULL)
		return NULL;
	return g_string_free(str, FALSE);
}
