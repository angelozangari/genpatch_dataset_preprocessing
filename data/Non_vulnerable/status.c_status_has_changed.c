static void
status_has_changed(PurpleStatus *status)
{
	PurplePresence *presence;
	PurpleStatus *old_status;
	presence   = purple_status_get_presence(status);
	/*
	 * If this status is exclusive, then we must be setting it to "active."
	 * Since we are setting it to active, we want to set the currently
	 * active status to "inactive."
	 */
	if (purple_status_is_exclusive(status))
	{
		old_status = purple_presence_get_active_status(presence);
		if (old_status != NULL && (old_status != status))
			old_status->active = FALSE;
		presence->active_status = status;
	}
	else
		old_status = NULL;
	notify_status_update(presence, old_status, status);
}
