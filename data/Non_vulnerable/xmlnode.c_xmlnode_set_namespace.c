}
void xmlnode_set_namespace(xmlnode *node, const char *xmlns)
{
	g_return_if_fail(node != NULL);
	g_free(node->xmlns);
	node->xmlns = g_strdup(xmlns);
}
