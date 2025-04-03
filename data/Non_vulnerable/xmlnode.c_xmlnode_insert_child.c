void
xmlnode_insert_child(xmlnode *parent, xmlnode *child)
{
	g_return_if_fail(parent != NULL);
	g_return_if_fail(child != NULL);
	child->parent = parent;
	if(parent->lastchild) {
		parent->lastchild->next = child;
	} else {
		parent->child = child;
	}
	parent->lastchild = child;
}
