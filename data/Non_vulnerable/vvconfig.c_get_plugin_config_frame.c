static GtkWidget *
get_plugin_config_frame(PurplePlugin *plugin) {
	GtkWidget *notebook, *vbox_audio, *vbox_video;
	GtkSizeGroup *sg;
	notebook = gtk_notebook_new();
	gtk_container_set_border_width(GTK_CONTAINER(notebook),
			PIDGIN_HIG_BORDER);
	gtk_widget_show(notebook);
	vbox_audio = gtk_vbox_new(FALSE, PIDGIN_HIG_CAT_SPACE);
	vbox_video = gtk_vbox_new(FALSE, PIDGIN_HIG_CAT_SPACE);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			vbox_audio, gtk_label_new(_("Audio")));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			vbox_video, gtk_label_new(_("Video")));
	gtk_container_set_border_width(GTK_CONTAINER (vbox_audio),
			PIDGIN_HIG_BORDER);
	gtk_container_set_border_width(GTK_CONTAINER (vbox_video),
			PIDGIN_HIG_BORDER);
	gtk_widget_show(vbox_audio);
	gtk_widget_show(vbox_video);
	sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	get_plugin_frame(vbox_audio, sg, _("Output"), _("_Plugin"), AUDIO_SINK_PLUGINS,
			"/plugins/core/vvconfig/audio/sink/plugin", _("_Device"),
			"/plugins/core/vvconfig/audio/sink/device");
	get_plugin_frame(vbox_audio, sg, _("Input"), _("P_lugin"), AUDIO_SRC_PLUGINS,
			"/plugins/core/vvconfig/audio/src/plugin", _("D_evice"),
			"/plugins/core/vvconfig/audio/src/device");
	get_plugin_frame(vbox_video, sg, _("Output"), _("_Plugin"), VIDEO_SINK_PLUGINS,
			"/plugins/gtk/vvconfig/video/sink/plugin", _("_Device"),
			"/plugins/gtk/vvconfig/video/sink/device");
	get_plugin_frame(vbox_video, sg, _("Input"), _("P_lugin"), VIDEO_SRC_PLUGINS,
			"/plugins/core/vvconfig/video/src/plugin", _("D_evice"),
			"/plugins/core/vvconfig/video/src/device");
	return notebook;
}
