}
static GtkWidget *wintrans_slider(GtkWidget *win) {
	GtkWidget *hbox;
	GtkWidget *label, *slider;
	GtkWidget *frame;
	int imalpha = purple_prefs_get_int(OPT_WINTRANS_IM_ALPHA);
	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
	gtk_widget_show(frame);
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), hbox);
	label = gtk_label_new(_("Opacity:"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_widget_show(hbox);
	slider = gtk_hscale_new_with_range(50, 255, 1);
	gtk_range_set_value(GTK_RANGE(slider), imalpha);
	gtk_widget_set_usize(GTK_WIDGET(slider), 200, -1);
	/* On slider val change, update window's transparency level */
	g_signal_connect(GTK_OBJECT(slider), "value-changed",
		G_CALLBACK(change_alpha), win);
	gtk_box_pack_start(GTK_BOX(hbox), slider, FALSE, TRUE, 5);
	/* Set the initial transparency level */
	set_wintrans(win, imalpha, TRUE,
		purple_prefs_get_bool(OPT_WINTRANS_IM_ONTOP));
	gtk_widget_show_all(hbox);
	return frame;
}
