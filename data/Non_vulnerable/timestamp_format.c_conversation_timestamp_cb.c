}
static char *conversation_timestamp_cb(PurpleConversation *conv,
                                       time_t t, gboolean show_date, gpointer data)
{
	const char *force = purple_prefs_get_string(
				"/plugins/gtk/timestamp_format/force");
	const char *dates = purple_prefs_get_string(
				"/plugins/gtk/timestamp_format/use_dates/conversation");
	g_return_val_if_fail(conv != NULL, NULL);
	return timestamp_cb_common(conv, t, show_date, force, dates, TRUE);
}
