void
xmlnode_set_attrib_with_prefix(xmlnode *node, const char *attr, const char *prefix, const char *value)
{
	xmlnode_set_attrib_full(node, attr, NULL, prefix, value);
}
