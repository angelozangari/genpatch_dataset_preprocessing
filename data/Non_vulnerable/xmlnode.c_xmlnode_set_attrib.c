void
xmlnode_set_attrib(xmlnode *node, const char *attr, const char *value)
{
	xmlnode_remove_attrib(node, attr);
	xmlnode_set_attrib_full(node, attr, NULL, NULL, value);
}
