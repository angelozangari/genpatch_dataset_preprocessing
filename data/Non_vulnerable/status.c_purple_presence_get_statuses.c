GList *
purple_presence_get_statuses(const PurplePresence *presence)
{
	g_return_val_if_fail(presence != NULL, NULL);
	return presence->statuses;
}
