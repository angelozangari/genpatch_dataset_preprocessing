}
void xmlnode_set_prefix(xmlnode *node, const char *prefix)
{
	g_return_if_fail(node != NULL);
	g_free(node->prefix);
	node->prefix = g_strdup(prefix);
}
