xmlnode *
xmlnode_get_child_with_namespace(const xmlnode *parent, const char *name, const char *ns)
{
	xmlnode *x, *ret = NULL;
	char **names;
	char *parent_name, *child_name;
	g_return_val_if_fail(parent != NULL, NULL);
	g_return_val_if_fail(name != NULL, NULL);
	names = g_strsplit(name, "/", 2);
	parent_name = names[0];
	child_name = names[1];
	for(x = parent->child; x; x = x->next) {
		/* XXX: Is it correct to ignore the namespace for the match if none was specified? */
		const char *xmlns = NULL;
		if(ns)
			xmlns = xmlnode_get_namespace(x);
		if(x->type == XMLNODE_TYPE_TAG && purple_strequal(parent_name, x->name)
				&& purple_strequal(ns, xmlns)) {
			ret = x;
			break;
		}
	}
	if(child_name && ret)
		ret = xmlnode_get_child(ret, child_name);
	g_strfreev(names);
	return ret;
}
