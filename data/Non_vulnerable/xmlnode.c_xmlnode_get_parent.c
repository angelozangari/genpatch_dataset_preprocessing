}
xmlnode *xmlnode_get_parent(const xmlnode *child)
{
	g_return_val_if_fail(child != NULL, NULL);
	return child->parent;
}
