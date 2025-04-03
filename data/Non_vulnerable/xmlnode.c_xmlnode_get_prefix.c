}
const char *xmlnode_get_prefix(const xmlnode *node)
{
	g_return_val_if_fail(node != NULL, NULL);
	return node->prefix;
}
