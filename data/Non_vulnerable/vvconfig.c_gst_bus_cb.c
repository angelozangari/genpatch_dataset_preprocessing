static gboolean
gst_bus_cb(GstBus *bus, GstMessage *msg, BusCbCtx *ctx)
{
	if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ELEMENT &&
		gst_structure_has_name(gst_message_get_structure(msg), "level")) {
		GstElement *src = GST_ELEMENT(GST_MESSAGE_SRC(msg));
		gchar *name = gst_element_get_name(src);
		if (!strcmp(name, "level")) {
			gdouble percent;
			gdouble threshold;
			GstElement *valve;
			percent = gst_msg_db_to_percent(msg, "rms");
			gtk_progress_bar_set_fraction(ctx->level, percent * 5);
			percent = gst_msg_db_to_percent(msg, "decay");
			threshold = gtk_range_get_value(ctx->threshold) / 100.0;
			valve = gst_bin_get_by_name(GST_BIN(GST_ELEMENT_PARENT(src)), "valve");
			g_object_set(valve, "drop", (percent < threshold), NULL);
			g_object_set(ctx->level,
					"text", (percent < threshold) ? _("DROP") : " ", NULL);
		}
		g_free(name);
	}
	return TRUE;
}
