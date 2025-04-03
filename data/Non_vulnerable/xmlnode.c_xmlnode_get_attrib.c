const char *
xmlnode_get_attrib(const xmlnode *node, const char *attr)
{
	xmlnode *x;
	g_return_val_if_fail(node != NULL, NULL);
	g_return_val_if_fail(attr != NULL, NULL);
	for(x = node->child; x; x = x->next) {
		if(x->type == XMLNODE_TYPE_ATTRIB && purple_strequal(attr, x->name)) {
			return x->data;
		}
	}
	return NULL;
}
