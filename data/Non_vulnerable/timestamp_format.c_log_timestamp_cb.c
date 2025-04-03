}
static char *log_timestamp_cb(PurpleLog *log, time_t t, gboolean show_date, gpointer data)
{
	const char *force = purple_prefs_get_string(
				"/plugins/gtk/timestamp_format/force");
	const char *dates = purple_prefs_get_string(
				"/plugins/gtk/timestamp_format/use_dates/log");
	g_return_val_if_fail(log != NULL, NULL);
	return timestamp_cb_common(log->conv, t, show_date, force, dates, FALSE);
}
