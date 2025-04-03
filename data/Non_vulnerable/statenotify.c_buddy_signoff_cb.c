static void
buddy_signoff_cb(PurpleBuddy *buddy, void *data)
{
	if (purple_prefs_get_bool("/plugins/core/statenotify/notify_signon"))
		write_status(buddy, _("%s has signed off."));
}
