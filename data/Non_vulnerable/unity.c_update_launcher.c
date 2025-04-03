static void
update_launcher()
{
	guint count = 0;
	GList *convs = NULL;
	g_return_if_fail(launcher != NULL);
	if (launcher_count == LAUNCHER_COUNT_DISABLE)
		return;
	if (launcher_count == LAUNCHER_COUNT_MESSAGES) {
		for (convs = purple_get_conversations(); convs != NULL; convs = convs->next) {
			PurpleConversation *conv = convs->data;
			count += GPOINTER_TO_INT(purple_conversation_get_data(conv,
			                         "unity-message-count"));
		}
	} else {
		count = n_sources;
	}
	if (launcher != NULL) {
		if (count > 0)
			unity_launcher_entry_set_count_visible(launcher, TRUE);
		else
			unity_launcher_entry_set_count_visible(launcher, FALSE);
		unity_launcher_entry_set_count(launcher, count);
	}
}
