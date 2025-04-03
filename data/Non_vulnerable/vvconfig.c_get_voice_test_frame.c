static GtkWidget *
get_voice_test_frame(PurplePlugin *plugin)
{
	GtkWidget *vbox = gtk_vbox_new(FALSE, PIDGIN_HIG_BORDER);
	GtkWidget *level = gtk_progress_bar_new();
	GtkWidget *volume = gtk_hscale_new_with_range(0, 100, 1);
	GtkWidget *threshold = gtk_hscale_new_with_range(0, 100, 1);
	GtkWidget *label;
	GtkTable *table = GTK_TABLE(gtk_table_new(2, 2, FALSE));
	GstElement *pipeline;
	GstBus *bus;
	BusCbCtx *ctx;
	g_object_set(vbox, "width-request", 500, NULL);
	gtk_table_set_row_spacings(table, PIDGIN_HIG_BOX_SPACE);
	gtk_table_set_col_spacings(table, PIDGIN_HIG_BOX_SPACE);
	label = gtk_label_new(_("Volume:"));
	g_object_set(label, "xalign", 0.0, NULL);
	gtk_table_attach(table, label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0);
	gtk_table_attach_defaults(table, volume, 1, 2, 0, 1);
	label = gtk_label_new(_("Silence threshold:"));
	g_object_set(label, "xalign", 0.0, "yalign", 1.0, NULL);
	gtk_table_attach(table, label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach_defaults(table, threshold, 1, 2, 1, 2);
	gtk_container_add(GTK_CONTAINER(vbox), level);
	gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(table));
	gtk_widget_show_all(vbox);
	pipeline = create_pipeline();
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_signal_watch(bus);
	ctx = g_new(BusCbCtx, 1);
	ctx->level = GTK_PROGRESS_BAR(level);
	ctx->threshold = GTK_RANGE(threshold);
	g_signal_connect_data(bus, "message", G_CALLBACK(gst_bus_cb),
			ctx, (GClosureNotify)g_free, 0);
	gst_object_unref(bus);
	g_signal_connect(volume, "value-changed",
			(GCallback)on_volume_change_cb, pipeline);
	gtk_range_set_value(GTK_RANGE(volume),
			purple_prefs_get_int("/purple/media/audio/volume/input"));
	gtk_widget_set(volume, "draw-value", FALSE, NULL);
	gtk_range_set_value(GTK_RANGE(threshold),
			purple_prefs_get_int("/purple/media/audio/silence_threshold"));
	g_signal_connect(vbox, "destroy",
			G_CALLBACK(voice_test_frame_destroy_cb), pipeline);
	g_signal_connect(volume, "destroy",
			G_CALLBACK(volume_scale_destroy_cb), NULL);
	g_signal_connect(threshold, "format-value",
			G_CALLBACK(threshold_value_format_cb), NULL);
	g_signal_connect(threshold, "destroy",
			G_CALLBACK(threshold_scale_destroy_cb), NULL);
	return vbox;
}
