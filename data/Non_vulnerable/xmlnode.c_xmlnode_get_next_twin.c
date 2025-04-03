xmlnode *
xmlnode_get_next_twin(xmlnode *node)
{
	xmlnode *sibling;
	const char *ns = xmlnode_get_namespace(node);
	g_return_val_if_fail(node != NULL, NULL);
	g_return_val_if_fail(node->type == XMLNODE_TYPE_TAG, NULL);
	for(sibling = node->next; sibling; sibling = sibling->next) {
		/* XXX: Is it correct to ignore the namespace for the match if none was specified? */
		const char *xmlns = NULL;
		if(ns)
			xmlns = xmlnode_get_namespace(sibling);
		if(sibling->type == XMLNODE_TYPE_TAG && purple_strequal(node->name, sibling->name) &&
				purple_strequal(ns, xmlns))
			return sibling;
	}
	return NULL;
}
