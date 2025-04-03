void
xmlnode_set_attrib_full(xmlnode *node, const char *attr, const char *xmlns, const char *prefix, const char *value)
{
	xmlnode *attrib_node;
	g_return_if_fail(node != NULL);
	g_return_if_fail(attr != NULL);
	g_return_if_fail(value != NULL);
	xmlnode_remove_attrib_with_namespace(node, attr, xmlns);
	attrib_node = new_node(attr, XMLNODE_TYPE_ATTRIB);
	attrib_node->data = g_strdup(value);
	attrib_node->xmlns = g_strdup(xmlns);
	attrib_node->prefix = g_strdup(prefix);
	xmlnode_insert_child(node, attrib_node);
}
