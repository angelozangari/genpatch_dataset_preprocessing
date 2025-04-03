#include "status.h"
static void jabber_nick_cb(JabberStream *js, const char *from, xmlnode *items) {
	/* it doesn't make sense to have more than one item here, so let's just pick the first one */
	xmlnode *item = xmlnode_get_child(items, "item");
	JabberBuddy *buddy = jabber_buddy_find(js, from, FALSE);
	xmlnode *nick;
	char *nickname = NULL;
	/* ignore the nick of people not on our buddy list */
	if (!buddy || !item)
		return;
	nick = xmlnode_get_child_with_namespace(item, "nick", "http://jabber.org/protocol/nick");
	if (!nick)
		return;
	nickname = xmlnode_get_data(nick);
	serv_got_alias(js->gc, from, nickname);
	g_free(nickname);
}
