static void
do_got_own_avatar_0_12_cb(JabberStream *js, const char *from, xmlnode *items)
{
	if (items)
		/* It wasn't an error (i.e. 'item-not-found') */
		remove_avatar_0_12_nodes(js);
}
