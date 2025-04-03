xmlnode*
xmlnode_new(const char *name)
{
	g_return_val_if_fail(name != NULL && *name != '\0', NULL);
	return new_node(name, XMLNODE_TYPE_TAG);
}
