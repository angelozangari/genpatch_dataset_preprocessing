void
xmlnode_free(xmlnode *node)
{
	xmlnode *x, *y;
	g_return_if_fail(node != NULL);
	/* if we're part of a tree, remove ourselves from the tree first */
	if(NULL != node->parent) {
		if(node->parent->child == node) {
			node->parent->child = node->next;
			if (node->parent->lastchild == node)
				node->parent->lastchild = node->next;
		} else {
			xmlnode *prev = node->parent->child;
			while(prev && prev->next != node) {
				prev = prev->next;
			}
			if(prev) {
				prev->next = node->next;
				if (node->parent->lastchild == node)
					node->parent->lastchild = prev;
			}
		}
	}
	/* now free our children */
	x = node->child;
	while(x) {
		y = x->next;
		xmlnode_free(x);
		x = y;
	}
	/* now dispose of ourselves */
	g_free(node->name);
	g_free(node->data);
	g_free(node->xmlns);
	g_free(node->prefix);
	if(node->namespace_map)
		g_hash_table_destroy(node->namespace_map);
	PURPLE_DBUS_UNREGISTER_POINTER(node);
	g_free(node);
}
