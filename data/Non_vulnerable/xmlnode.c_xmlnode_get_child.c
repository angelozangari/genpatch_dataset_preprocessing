xmlnode*
xmlnode_get_child(const xmlnode *parent, const char *name)
{
	return xmlnode_get_child_with_namespace(parent, name, NULL);
}
