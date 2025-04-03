void
purple_presence_switch_status(PurplePresence *presence, const char *status_id)
{
	purple_presence_set_status_active(presence, status_id, TRUE);
}
