static void
threshold_scale_destroy_cb(GtkRange *threshold, gpointer nul)
{
	purple_prefs_set_int("/purple/media/audio/silence_threshold",
			gtk_range_get_value(threshold));
}
