}
static PurpleChat *zephyr_find_blist_chat(PurpleAccount *account, const char *name)
{
	PurpleBlistNode *gnode, *cnode;
	/* XXX needs to be %host%,%canon%, and %me% clean */
	for(gnode = purple_blist_get_root(); gnode;
			gnode = purple_blist_node_get_sibling_next(gnode)) {
		for(cnode = purple_blist_node_get_first_child(gnode);
				cnode;
				cnode = purple_blist_node_get_sibling_next(cnode)) {
			PurpleChat *chat = (PurpleChat*)cnode;
			char *zclass, *inst, *recip;
			char** triple;
			GHashTable *components;
			if(!PURPLE_BLIST_NODE_IS_CHAT(cnode))
				continue;
			if(purple_chat_get_account(chat) != account)
				continue;
			components = purple_chat_get_components(chat);
			if(!(zclass = g_hash_table_lookup(components, "class")))
				continue;
			if(!(inst = g_hash_table_lookup(components, "instance")))
				inst = g_strdup("");
			if(!(recip = g_hash_table_lookup(components, "recipient")))
				recip = g_strdup("");
			/*			purple_debug_info("zephyr","in zephyr_find_blist_chat name: %s\n",name?name:""); */
			triple = g_strsplit(name,",",3);
			if (!g_ascii_strcasecmp(triple[0],zclass) && !g_ascii_strcasecmp(triple[1],inst) && !g_ascii_strcasecmp(triple[2],recip))
				return chat;
		}
	}
	return NULL;
}
