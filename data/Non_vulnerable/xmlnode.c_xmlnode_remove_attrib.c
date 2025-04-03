void
xmlnode_remove_attrib(xmlnode *node, const char *attr)
{
	xmlnode *attr_node, *sibling = NULL;
	g_return_if_fail(node != NULL);
	g_return_if_fail(attr != NULL);
	attr_node = node->child;
	while (attr_node) {
		if(attr_node->type == XMLNODE_TYPE_ATTRIB &&
				purple_strequal(attr_node->name, attr))
		{
			if (node->lastchild == attr_node) {
				node->lastchild = sibling;
			}
			if (sibling == NULL) {
				node->child = attr_node->next;
				xmlnode_free(attr_node);
				attr_node = node->child;
			} else {
				sibling->next = attr_node->next;
				sibling = attr_node->next;
				xmlnode_free(attr_node);
				attr_node = sibling;
			}
		}
		else
		{
			attr_node = attr_node->next;
		}
		sibling = attr_node;
	}
}
