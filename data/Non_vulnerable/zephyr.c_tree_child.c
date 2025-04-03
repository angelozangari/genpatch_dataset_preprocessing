}
static parse_tree *tree_child(parse_tree* tree,int index) {
	if (index < tree->num_children) {
		return tree->children[index];
	} else {
		return &null_parse_tree;
	}
}
}
static parse_tree *tree_child(parse_tree* tree,int index) {
	if (index < tree->num_children) {
		return tree->children[index];
	} else {
		return &null_parse_tree;
	}
}
