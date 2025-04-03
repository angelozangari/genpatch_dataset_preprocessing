#include "status.h"
static void jabber_tune_cb(JabberStream *js, const char *from, xmlnode *items) {
	/* it doesn't make sense to have more than one item here, so let's just pick the first one */
	xmlnode *item = xmlnode_get_child(items, "item");
	JabberBuddy *buddy = jabber_buddy_find(js, from, FALSE);
	xmlnode *tuneinfo, *tune;
	PurpleJabberTuneInfo tuneinfodata;
	JabberBuddyResource *resource;
	gboolean valid = FALSE;
	/* ignore the tune of people not on our buddy list */
	if (!buddy || !item)
		return;
	tuneinfodata.artist = NULL;
	tuneinfodata.title = NULL;
	tuneinfodata.album = NULL;
	tuneinfodata.track = NULL;
	tuneinfodata.time = -1;
	tuneinfodata.url = NULL;
	tune = xmlnode_get_child_with_namespace(item, "tune", "http://jabber.org/protocol/tune");
	if (!tune)
		return;
	resource = jabber_buddy_find_resource(buddy, NULL);
	if(!resource)
		return; /* huh? */
	for (tuneinfo = tune->child; tuneinfo; tuneinfo = tuneinfo->next) {
		if (tuneinfo->type == XMLNODE_TYPE_TAG) {
			if (!strcmp(tuneinfo->name, "artist")) {
				if (tuneinfodata.artist == NULL) /* only pick the first one */
					tuneinfodata.artist = xmlnode_get_data(tuneinfo);
				valid = TRUE;
			} else if (!strcmp(tuneinfo->name, "length")) {
				if (tuneinfodata.time == -1) {
					char *length = xmlnode_get_data(tuneinfo);
					if (length)
						tuneinfodata.time = strtol(length, NULL, 10);
					g_free(length);
					if (tuneinfodata.time > 0)
						valid = TRUE;
				}
			} else if (!strcmp(tuneinfo->name, "source")) {
				if (tuneinfodata.album == NULL) /* only pick the first one */
					tuneinfodata.album = xmlnode_get_data(tuneinfo);
				valid = TRUE;
			} else if (!strcmp(tuneinfo->name, "title")) {
				if (tuneinfodata.title == NULL) /* only pick the first one */
					tuneinfodata.title = xmlnode_get_data(tuneinfo);
				valid = TRUE;
			} else if (!strcmp(tuneinfo->name, "track")) {
				if (tuneinfodata.track == NULL) /* only pick the first one */
					tuneinfodata.track = xmlnode_get_data(tuneinfo);
				valid = TRUE;
			} else if (!strcmp(tuneinfo->name, "uri")) {
				if (tuneinfodata.url == NULL) /* only pick the first one */
					tuneinfodata.url = xmlnode_get_data(tuneinfo);
				valid = TRUE;
			}
		}
	}
	if (valid) {
		purple_prpl_got_user_status(js->gc->account, from, "tune",
				PURPLE_TUNE_ARTIST, tuneinfodata.artist,
				PURPLE_TUNE_TITLE, tuneinfodata.title,
				PURPLE_TUNE_ALBUM, tuneinfodata.album,
				PURPLE_TUNE_TRACK, tuneinfodata.track,
				PURPLE_TUNE_TIME, tuneinfodata.time,
				PURPLE_TUNE_URL, tuneinfodata.url, NULL);
	} else {
		purple_prpl_got_user_status_deactive(js->gc->account, from, "tune");
	}
	g_free(tuneinfodata.artist);
	g_free(tuneinfodata.title);
	g_free(tuneinfodata.album);
	g_free(tuneinfodata.track);
	g_free(tuneinfodata.url);
}
