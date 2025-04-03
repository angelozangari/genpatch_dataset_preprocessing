static int
purple_presence_compute_score(const PurplePresence *presence)
{
	GList *l;
	int score = 0;
	for (l = purple_presence_get_statuses(presence); l != NULL; l = l->next) {
		PurpleStatus *status = (PurpleStatus *)l->data;
		PurpleStatusType *type = purple_status_get_type(status);
		if (purple_status_is_active(status)) {
			score += primitive_scores[purple_status_type_get_primitive(type)];
			if (!purple_status_is_online(status)) {
				PurpleBuddy *b = purple_presence_get_buddy(presence);
				if (b && purple_account_supports_offline_message(purple_buddy_get_account(b), b))
					score += primitive_scores[SCORE_OFFLINE_MESSAGE];
			}
		}
	}
	score += purple_account_get_int(purple_presence_get_account(presence), "score", 0);
	if (purple_presence_is_idle(presence))
		score += primitive_scores[SCORE_IDLE];
	return score;
}
