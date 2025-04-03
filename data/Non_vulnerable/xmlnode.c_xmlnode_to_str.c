char *
xmlnode_to_str(const xmlnode *node, int *len)
{
	return xmlnode_to_str_helper(node, len, FALSE, 0);
}
