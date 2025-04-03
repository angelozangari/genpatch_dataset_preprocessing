};
static void jabber_mood_cb(JabberStream *js, const char *from, xmlnode *items) {
	/* it doesn't make sense to have more than one item here, so let's just pick the first one */
	xmlnode *item = xmlnode_get_child(items, "item");
	const char *newmood = NULL;
	char *moodtext = NULL;
	JabberBuddy *buddy = jabber_buddy_find(js, from, FALSE);
	xmlnode *moodinfo, *mood;
	/* ignore the mood of people not on our buddy list */
	if (!buddy || !item)
		return;
	mood = xmlnode_get_child_with_namespace(item, "mood", "http://jabber.org/protocol/mood");
	if (!mood)
		return;
	for (moodinfo = mood->child; moodinfo; moodinfo = moodinfo->next) {
		if (moodinfo->type == XMLNODE_TYPE_TAG) {
			if (!strcmp(moodinfo->name, "text")) {
				if (!moodtext) /* only pick the first one */
					moodtext = xmlnode_get_data(moodinfo);
			} else {
				int i;
				for (i = 0; moods[i].mood; ++i) {
					/* verify that the mood is known (valid) */
					if (!strcmp(moodinfo->name, moods[i].mood)) {
						newmood = moods[i].mood;
						break;
					}
				}
			}
		}
		if (newmood != NULL && moodtext != NULL)
			break;
	}
	if (newmood != NULL) {
		purple_prpl_got_user_status(js->gc->account, from, "mood",
				PURPLE_MOOD_NAME, newmood,
				PURPLE_MOOD_COMMENT, moodtext,
				NULL);
	} else {
		purple_prpl_got_user_status_deactive(js->gc->account, from, "mood");
	}
	g_free(moodtext);
}
