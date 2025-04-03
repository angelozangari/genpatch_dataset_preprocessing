void
xmlnode_set_attrib_with_namespace(xmlnode *node, const char *attr, const char *xmlns, const char *value)
{
	xmlnode_set_attrib_full(node, attr, xmlns, NULL, value);
}
