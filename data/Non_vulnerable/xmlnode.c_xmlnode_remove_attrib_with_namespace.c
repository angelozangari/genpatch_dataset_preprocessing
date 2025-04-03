void
xmlnode_remove_attrib_with_namespace(xmlnode *node, const char *attr, const char *xmlns)
{
	xmlnode *attr_node, *sibling = NULL;
	g_return_if_fail(node != NULL);
	g_return_if_fail(attr != NULL);
	for(attr_node = node->child; attr_node; attr_node = attr_node->next)
	{
		if(attr_node->type == XMLNODE_TYPE_ATTRIB &&
		   purple_strequal(attr,  attr_node->name) &&
		   purple_strequal(xmlns, attr_node->xmlns))
		{
			if(sibling == NULL) {
				node->child = attr_node->next;
			} else {
				sibling->next = attr_node->next;
			}
			if (node->lastchild == attr_node) {
				node->lastchild = sibling;
			}
			xmlnode_free(attr_node);
			return;
		}
		sibling = attr_node;
	}
}
