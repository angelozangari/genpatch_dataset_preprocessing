}
void jabber_mood_set(JabberStream *js, const char *mood, const char *text) {
	xmlnode *publish, *moodnode;
	publish = xmlnode_new("publish");
	xmlnode_set_attrib(publish,"node","http://jabber.org/protocol/mood");
	moodnode = xmlnode_new_child(xmlnode_new_child(publish, "item"), "mood");
	xmlnode_set_namespace(moodnode, "http://jabber.org/protocol/mood");
	if (mood && *mood) {
		/* if mood is NULL, set an empty mood node, meaning: unset mood */
	    xmlnode_new_child(moodnode, mood);
	}
	if (text && *text) {
		xmlnode *textnode = xmlnode_new_child(moodnode, "text");
		xmlnode_insert_data(textnode, text, -1);
	}
	jabber_pep_publish(js, publish);
	/* publish is freed by jabber_pep_publish -> jabber_iq_send -> jabber_iq_free
	   (yay for well-defined memory management rules) */
}
