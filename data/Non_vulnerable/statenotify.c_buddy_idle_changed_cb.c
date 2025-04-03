static void
buddy_idle_changed_cb(PurpleBuddy *buddy, gboolean old_idle, gboolean idle,
                      void *data)
{
	if (purple_prefs_get_bool("/plugins/core/statenotify/notify_idle")) {
		if (idle && !old_idle) {
			write_status(buddy, _("%s has become idle."));
		} else if (!idle && old_idle) {
			write_status(buddy, _("%s is no longer idle."));
		}
	}
}
