static void
on_volume_change_cb(GtkRange *range, GstBin *pipeline)
{
	GstElement *volume;
	g_return_if_fail(pipeline != NULL);
	volume = gst_bin_get_by_name(pipeline, "volume");
	g_object_set(volume, "volume", gtk_range_get_value(range) / 10.0, NULL);
}
