}
void jabber_mood_init(void) {
	jabber_add_feature("http://jabber.org/protocol/mood", jabber_pep_namespace_only_when_pep_enabled_cb);
	jabber_pep_register_handler("http://jabber.org/protocol/mood", jabber_mood_cb);
}
