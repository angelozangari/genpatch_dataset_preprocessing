}
void jabber_avatar_fetch_mine(JabberStream *js)
{
	if (js->initial_avatar_hash) {
		jabber_pep_request_item(js, NULL, NS_AVATAR_0_12_METADATA, NULL,
		                        do_got_own_avatar_0_12_cb);
		jabber_pep_request_item(js, NULL, NS_AVATAR_1_1_METADATA, NULL,
		                        do_got_own_avatar_cb);
	}
}
