xmlnode *
xmlnode_copy(const xmlnode *src)
{
	xmlnode *ret;
	xmlnode *child;
	xmlnode *sibling = NULL;
	g_return_val_if_fail(src != NULL, NULL);
	ret = new_node(src->name, src->type);
	ret->xmlns = g_strdup(src->xmlns);
	if (src->data) {
		if (src->data_sz) {
			ret->data = g_memdup(src->data, src->data_sz);
			ret->data_sz = src->data_sz;
		} else {
			ret->data = g_strdup(src->data);
		}
	}
	ret->prefix = g_strdup(src->prefix);
	if (src->namespace_map) {
		ret->namespace_map = g_hash_table_new_full(g_str_hash, g_str_equal,
		                                           g_free, g_free);
		g_hash_table_foreach(src->namespace_map, xmlnode_copy_foreach_ns, ret->namespace_map);
	}
	for (child = src->child; child; child = child->next) {
		if (sibling) {
			sibling->next = xmlnode_copy(child);
			sibling = sibling->next;
		} else {
			ret->child = sibling = xmlnode_copy(child);
		}
		sibling->parent = ret;
	}
	ret->lastchild = sibling;
	return ret;
}
