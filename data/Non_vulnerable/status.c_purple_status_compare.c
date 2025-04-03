gint
purple_status_compare(const PurpleStatus *status1, const PurpleStatus *status2)
{
	PurpleStatusType *type1, *type2;
	int score1 = 0, score2 = 0;
	if ((status1 == NULL && status2 == NULL) ||
			(status1 == status2))
	{
		return 0;
	}
	else if (status1 == NULL)
		return 1;
	else if (status2 == NULL)
		return -1;
	type1 = purple_status_get_type(status1);
	type2 = purple_status_get_type(status2);
	if (purple_status_is_active(status1))
		score1 = primitive_scores[purple_status_type_get_primitive(type1)];
	if (purple_status_is_active(status2))
		score2 = primitive_scores[purple_status_type_get_primitive(type2)];
	if (score1 > score2)
		return -1;
	else if (score1 < score2)
		return 1;
	return 0;
}
