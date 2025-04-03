static void
buddy_status_changed_cb(PurpleBuddy *buddy, PurpleStatus *old_status,
                        PurpleStatus *status, void *data)
{
	gboolean available, old_available;
	if (!purple_status_is_exclusive(status) ||
			!purple_status_is_exclusive(old_status))
		return;
	available = purple_status_is_available(status);
	old_available = purple_status_is_available(old_status);
	if (purple_prefs_get_bool("/plugins/core/statenotify/notify_away")) {
		if (available && !old_available)
			write_status(buddy, _("%s is no longer away."));
		else if (!available && old_available)
			write_status(buddy, _("%s has gone away."));
	}
}
