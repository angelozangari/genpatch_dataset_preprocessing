const char *
xmlnode_get_attrib_with_namespace(const xmlnode *node, const char *attr, const char *xmlns)
{
	const xmlnode *x;
	g_return_val_if_fail(node != NULL, NULL);
	g_return_val_if_fail(attr != NULL, NULL);
	for(x = node->child; x; x = x->next) {
		if(x->type == XMLNODE_TYPE_ATTRIB &&
		   purple_strequal(attr,  x->name) &&
		   purple_strequal(xmlns, x->xmlns)) {
			return x->data;
		}
	}
	return NULL;
}
