static void
volume_scale_destroy_cb(GtkRange *volume, gpointer nul)
{
	purple_prefs_set_int("/purple/media/audio/volume/input",
			gtk_range_get_value(volume));
}
