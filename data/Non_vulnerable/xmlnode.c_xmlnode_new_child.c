xmlnode *
xmlnode_new_child(xmlnode *parent, const char *name)
{
	xmlnode *node;
	g_return_val_if_fail(parent != NULL, NULL);
	g_return_val_if_fail(name != NULL && *name != '\0', NULL);
	node = new_node(name, XMLNODE_TYPE_TAG);
	xmlnode_insert_child(parent, node);
	return node;
}
