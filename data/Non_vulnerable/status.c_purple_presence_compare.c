gint
purple_presence_compare(const PurplePresence *presence1,
		const PurplePresence *presence2)
{
	time_t idle_time_1, idle_time_2;
	int score1 = 0, score2 = 0;
	if (presence1 == presence2)
		return 0;
	else if (presence1 == NULL)
		return 1;
	else if (presence2 == NULL)
		return -1;
	if (purple_presence_is_online(presence1) &&
			!purple_presence_is_online(presence2))
		return -1;
	else if (purple_presence_is_online(presence2) &&
			!purple_presence_is_online(presence1))
		return 1;
	/* Compute the score of the first set of statuses. */
	score1 = purple_presence_compute_score(presence1);
	/* Compute the score of the second set of statuses. */
	score2 = purple_presence_compute_score(presence2);
	idle_time_1 = time(NULL) - purple_presence_get_idle_time(presence1);
	idle_time_2 = time(NULL) - purple_presence_get_idle_time(presence2);
	if (idle_time_1 > idle_time_2)
		score1 += primitive_scores[SCORE_IDLE_TIME];
	else if (idle_time_1 < idle_time_2)
		score2 += primitive_scores[SCORE_IDLE_TIME];
	if (score1 < score2)
		return 1;
	else if (score1 > score2)
		return -1;
	return 0;
}
