static void
remove_avatar_0_12_nodes(JabberStream *js)
{
#if 0
	/* See note below for why this is #if 0'd */
	/* Publish an empty avatar according to the XEP-0084 v0.12 semantics */
	xmlnode *publish, *item, *metadata;
	/* publish the metadata */
	publish = xmlnode_new("publish");
	xmlnode_set_attrib(publish, "node", NS_AVATAR_0_12_METADATA);
	item = xmlnode_new_child(publish, "item");
	xmlnode_set_attrib(item, "id", "stop");
	metadata = xmlnode_new_child(item, "metadata");
	xmlnode_set_namespace(metadata, NS_AVATAR_0_12_METADATA);
	xmlnode_new_child(metadata, "stop");
	/* publish */
	jabber_pep_publish(js, publish);
#endif
	/*
	 * This causes ejabberd 2.0.0 to kill the connection unceremoniously.
	 * See https://support.process-one.net/browse/EJAB-623. When adiumx.com
	 * was upgraded, the issue went away.
	 *
	 * I think it makes a lot of sense to not have an avatar at the old
	 * node instead of having something interpreted as "no avatar". When
	 * a contact with an older client logs in, in the latter situation,
	 * there's a race between interpreting the <presence/> vcard-temp:x:update
	 * avatar (non-empty) and the XEP-0084 v0.12 avatar (empty, so show no
	 * avatar for the buddy) which leads to unhappy and confused users.
	 *
	 * A deluge of frustrating "Read error" bug reports may change my mind
	 * about this.
	 * --darkrain42
	 */
	jabber_pep_delete_node(js, NS_AVATAR_0_12_METADATA);
	jabber_pep_delete_node(js, NS_AVATAR_0_12_DATA);
}
