void
purple_status_init(void)
{
	void *handle = purple_status_get_handle();
	purple_prefs_add_none("/purple/status");
	purple_prefs_add_none("/purple/status/scores");
	purple_prefs_add_int("/purple/status/scores/offline",
			primitive_scores[PURPLE_STATUS_OFFLINE]);
	purple_prefs_add_int("/purple/status/scores/available",
			primitive_scores[PURPLE_STATUS_AVAILABLE]);
	purple_prefs_add_int("/purple/status/scores/invisible",
			primitive_scores[PURPLE_STATUS_INVISIBLE]);
	purple_prefs_add_int("/purple/status/scores/away",
			primitive_scores[PURPLE_STATUS_AWAY]);
	purple_prefs_add_int("/purple/status/scores/extended_away",
			primitive_scores[PURPLE_STATUS_EXTENDED_AWAY]);
	purple_prefs_add_int("/purple/status/scores/idle",
			primitive_scores[SCORE_IDLE]);
	purple_prefs_add_int("/purple/status/scores/offline_msg",
			primitive_scores[SCORE_OFFLINE_MESSAGE]);
	purple_prefs_connect_callback(handle, "/purple/status/scores/offline",
			score_pref_changed_cb,
			GINT_TO_POINTER(PURPLE_STATUS_OFFLINE));
	purple_prefs_connect_callback(handle, "/purple/status/scores/available",
			score_pref_changed_cb,
			GINT_TO_POINTER(PURPLE_STATUS_AVAILABLE));
	purple_prefs_connect_callback(handle, "/purple/status/scores/invisible",
			score_pref_changed_cb,
			GINT_TO_POINTER(PURPLE_STATUS_INVISIBLE));
	purple_prefs_connect_callback(handle, "/purple/status/scores/away",
			score_pref_changed_cb,
			GINT_TO_POINTER(PURPLE_STATUS_AWAY));
	purple_prefs_connect_callback(handle, "/purple/status/scores/extended_away",
			score_pref_changed_cb,
			GINT_TO_POINTER(PURPLE_STATUS_EXTENDED_AWAY));
	purple_prefs_connect_callback(handle, "/purple/status/scores/idle",
			score_pref_changed_cb,
			GINT_TO_POINTER(SCORE_IDLE));
	purple_prefs_connect_callback(handle, "/purple/status/scores/offline_msg",
			score_pref_changed_cb,
			GINT_TO_POINTER(SCORE_OFFLINE_MESSAGE));
	purple_prefs_trigger_callback("/purple/status/scores/offline");
	purple_prefs_trigger_callback("/purple/status/scores/available");
	purple_prefs_trigger_callback("/purple/status/scores/invisible");
	purple_prefs_trigger_callback("/purple/status/scores/away");
	purple_prefs_trigger_callback("/purple/status/scores/extended_away");
	purple_prefs_trigger_callback("/purple/status/scores/idle");
	purple_prefs_trigger_callback("/purple/status/scores/offline_msg");
}
