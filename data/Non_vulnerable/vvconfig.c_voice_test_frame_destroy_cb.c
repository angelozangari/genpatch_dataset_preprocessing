static void
voice_test_frame_destroy_cb(GtkObject *w, GstElement *pipeline)
{
	g_return_if_fail(GST_IS_ELEMENT(pipeline));
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}
