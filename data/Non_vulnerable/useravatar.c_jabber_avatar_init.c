static void update_buddy_metadata(JabberStream *js, const char *from, xmlnode *items);
void jabber_avatar_init(void)
{
	jabber_add_feature(NS_AVATAR_1_1_METADATA,
	                   jabber_pep_namespace_only_when_pep_enabled_cb);
	jabber_add_feature(NS_AVATAR_1_1_DATA,
	                   jabber_pep_namespace_only_when_pep_enabled_cb);
	jabber_pep_register_handler(NS_AVATAR_1_1_METADATA,
	                            update_buddy_metadata);
}
