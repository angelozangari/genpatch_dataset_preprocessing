static xmlnode*
new_node(const char *name, XMLNodeType type)
{
	xmlnode *node = g_new0(xmlnode, 1);
	node->name = g_strdup(name);
	node->type = type;
	PURPLE_DBUS_REGISTER_POINTER(node, xmlnode);
	return node;
}
