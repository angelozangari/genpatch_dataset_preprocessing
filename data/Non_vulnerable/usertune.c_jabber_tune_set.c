}
void jabber_tune_set(PurpleConnection *gc, const PurpleJabberTuneInfo *tuneinfo) {
	xmlnode *publish, *tunenode;
	JabberStream *js = gc->proto_data;
	publish = xmlnode_new("publish");
	xmlnode_set_attrib(publish,"node","http://jabber.org/protocol/tune");
	tunenode = xmlnode_new_child(xmlnode_new_child(publish, "item"), "tune");
	xmlnode_set_namespace(tunenode, "http://jabber.org/protocol/tune");
	if(tuneinfo) {
		if(tuneinfo->artist && tuneinfo->artist[0] != '\0')
			xmlnode_insert_data(xmlnode_new_child(tunenode, "artist"),tuneinfo->artist,-1);
		if(tuneinfo->title && tuneinfo->title[0] != '\0')
			xmlnode_insert_data(xmlnode_new_child(tunenode, "title"),tuneinfo->title,-1);
		if(tuneinfo->album && tuneinfo->album[0] != '\0')
			xmlnode_insert_data(xmlnode_new_child(tunenode, "source"),tuneinfo->album,-1);
		if(tuneinfo->url && tuneinfo->url[0] != '\0')
			xmlnode_insert_data(xmlnode_new_child(tunenode, "uri"),tuneinfo->url,-1);
		if(tuneinfo->time > 0) {
			char *length = g_strdup_printf("%d", tuneinfo->time);
			xmlnode_insert_data(xmlnode_new_child(tunenode, "length"),length,-1);
			g_free(length);
		}
		if(tuneinfo->track && tuneinfo->track[0] != '\0')
			xmlnode_insert_data(xmlnode_new_child(tunenode, "track"),tuneinfo->track,-1);
	}
	jabber_pep_publish(js, publish);
	/* publish is freed by jabber_pep_publish -> jabber_iq_send -> jabber_iq_free
	   (yay for well-defined memory management rules) */
}
