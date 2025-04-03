}
static parse_tree *find_node(parse_tree* ptree,gchar* key)
{
	gchar* tc;
	if (!ptree || ! key)
		return &null_parse_tree;
	tc = tree_child(ptree,0)->contents;
	/* g_strcasecmp() is deprecated.  What is the encoding here??? */
	if (ptree->num_children > 0  &&	tc && !g_ascii_strcasecmp(tc, key)) {
		return ptree;
	} else {
		parse_tree *result = &null_parse_tree;
		int i;
		for(i = 0; i < ptree->num_children; i++) {
			result = find_node(ptree->children[i],key);
			if(result != &null_parse_tree) {
				break;
			}
		}
		return result;
	}
}
