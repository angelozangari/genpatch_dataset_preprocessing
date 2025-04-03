static void
do_buddy_avatar_update_data(JabberStream *js, const char *from, xmlnode *items)
{
	xmlnode *item, *data;
	const char *checksum;
	char *b64data;
	void *img;
	size_t size;
	if(!items)
		return;
	item = xmlnode_get_child(items, "item");
	if(!item)
		return;
	data = xmlnode_get_child(item, "data");
	if(!data)
		return;
	checksum = xmlnode_get_attrib(item,"id");
	if(!checksum)
		return;
	b64data = xmlnode_get_data(data);
	if(!b64data)
		return;
	img = purple_base64_decode(b64data, &size);
	if(!img) {
		g_free(b64data);
		return;
	}
	purple_buddy_icons_set_for_user(purple_connection_get_account(js->gc), from, img, size, checksum);
	g_free(b64data);
}
