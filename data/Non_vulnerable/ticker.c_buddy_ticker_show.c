}
static void buddy_ticker_show(void)
{
	PurpleBlistNode *gnode, *cnode, *bnode;
	PurpleBuddy *b;
	for(gnode = purple_blist_get_root();
	    gnode;
	    gnode = purple_blist_node_get_sibling_next(gnode))
	{
		if(!PURPLE_BLIST_NODE_IS_GROUP(gnode))
			continue;
		for(cnode = purple_blist_node_get_first_child(gnode);
		    cnode;
		    cnode = purple_blist_node_get_sibling_next(cnode))
		{
			if(!PURPLE_BLIST_NODE_IS_CONTACT(cnode))
				continue;
			for(bnode = purple_blist_node_get_first_child(cnode);
			    bnode;
			    bnode = purple_blist_node_get_sibling_next(bnode))
			{
				if(!PURPLE_BLIST_NODE_IS_BUDDY(bnode))
					continue;
				b = (PurpleBuddy *)bnode;
				if(PURPLE_BUDDY_IS_ONLINE(b))
					buddy_ticker_add_buddy(b);
			}
		}
	}
}
