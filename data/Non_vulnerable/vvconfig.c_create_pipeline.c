static GstElement *
create_pipeline()
{
	GstElement *pipeline = gst_pipeline_new("voicetest");
	GstElement *src = create_audio_src(NULL, NULL, NULL);
	GstElement *sink = create_audio_sink(NULL, NULL, NULL);
	GstElement *volume = gst_element_factory_make("volume", "volume");
	GstElement *level = gst_element_factory_make("level", "level");
	GstElement *valve = gst_element_factory_make("valve", "valve");
	gst_bin_add_many(GST_BIN(pipeline), src, volume, level, valve, sink, NULL);
	gst_element_link_many(src, volume, level, valve, sink, NULL);
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
	return pipeline;
}
