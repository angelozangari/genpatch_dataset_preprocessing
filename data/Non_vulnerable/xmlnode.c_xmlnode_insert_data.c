void
xmlnode_insert_data(xmlnode *node, const char *data, gssize size)
{
	xmlnode *child;
	gsize real_size;
	g_return_if_fail(node != NULL);
	g_return_if_fail(data != NULL);
	g_return_if_fail(size != 0);
	real_size = size == -1 ? strlen(data) : (gsize)size;
	child = new_node(NULL, XMLNODE_TYPE_DATA);
	child->data = g_memdup(data, real_size);
	child->data_sz = real_size;
	xmlnode_insert_child(node, child);
}
