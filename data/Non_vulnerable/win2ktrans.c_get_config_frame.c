}
static GtkWidget *get_config_frame(PurplePlugin *plugin) {
	GtkWidget *ret;
	GtkWidget *imtransbox, *bltransbox;
	GtkWidget *hbox;
	GtkWidget *label, *slider;
	GtkWidget *button;
	GtkWidget *trans_box;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width(GTK_CONTAINER (ret), 12);
	/* IM Convo trans options */
	imtransbox = pidgin_make_frame(ret, _("IM Conversation Windows"));
	button = pidgin_prefs_checkbox(_("_IM window transparency"),
		OPT_WINTRANS_IM_ENABLED, imtransbox);
	g_signal_connect(GTK_OBJECT(button), "clicked",
		G_CALLBACK(update_convs_wintrans),
		(gpointer) OPT_WINTRANS_IM_ENABLED);
	trans_box = gtk_vbox_new(FALSE, 18);
	if (!purple_prefs_get_bool(OPT_WINTRANS_IM_ENABLED))
		gtk_widget_set_sensitive(GTK_WIDGET(trans_box), FALSE);
	gtk_widget_show(trans_box);
	g_signal_connect(GTK_OBJECT(button), "clicked",
		G_CALLBACK(pidgin_toggle_sensitive), trans_box);
	button = pidgin_prefs_checkbox(_("_Show slider bar in IM window"),
		OPT_WINTRANS_IM_SLIDER, trans_box);
	g_signal_connect(GTK_OBJECT(button), "clicked",
		G_CALLBACK(update_convs_wintrans),
		(gpointer) OPT_WINTRANS_IM_SLIDER);
	button = pidgin_prefs_checkbox(
		_("Remove IM window transparency on focus"),
		OPT_WINTRANS_IM_ONFOCUS, trans_box);
	button = pidgin_prefs_checkbox(_("Always on top"), OPT_WINTRANS_IM_ONTOP,
		trans_box);
	g_signal_connect(GTK_OBJECT(button), "clicked",
		G_CALLBACK(update_convs_wintrans),
		(gpointer) OPT_WINTRANS_IM_ONTOP);
	gtk_box_pack_start(GTK_BOX(imtransbox), trans_box, FALSE, FALSE, 5);
	/* IM transparency slider */
	hbox = gtk_hbox_new(FALSE, 5);
	label = gtk_label_new(_("Opacity:"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	slider = gtk_hscale_new_with_range(50, 255, 1);
	gtk_range_set_value(GTK_RANGE(slider),
		purple_prefs_get_int(OPT_WINTRANS_IM_ALPHA));
	gtk_widget_set_usize(GTK_WIDGET(slider), 200, -1);
	g_signal_connect(GTK_OBJECT(slider), "value-changed",
		G_CALLBACK(alpha_change), NULL);
	g_signal_connect(GTK_OBJECT(slider), "focus-out-event",
		G_CALLBACK(alpha_pref_set_int),
		(gpointer) OPT_WINTRANS_IM_ALPHA);
	gtk_box_pack_start(GTK_BOX(hbox), slider, FALSE, TRUE, 5);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(trans_box), hbox, FALSE, FALSE, 5);
	/* Buddy List trans options */
	bltransbox = pidgin_make_frame (ret, _("Buddy List Window"));
	button = pidgin_prefs_checkbox(_("_Buddy List window transparency"),
		OPT_WINTRANS_BL_ENABLED, bltransbox);
	g_signal_connect(GTK_OBJECT(button), "clicked",
		G_CALLBACK(set_blist_trans),
		(gpointer) OPT_WINTRANS_BL_ENABLED);
	trans_box = gtk_vbox_new(FALSE, 18);
	if (!purple_prefs_get_bool(OPT_WINTRANS_BL_ENABLED))
		gtk_widget_set_sensitive(GTK_WIDGET(trans_box), FALSE);
	gtk_widget_show(trans_box);
	g_signal_connect(GTK_OBJECT(button), "clicked",
		G_CALLBACK(pidgin_toggle_sensitive), trans_box);
	button = pidgin_prefs_checkbox(
		_("Remove Buddy List window transparency on focus"),
		OPT_WINTRANS_BL_ONFOCUS, trans_box);
	button = pidgin_prefs_checkbox(_("Always on top"), OPT_WINTRANS_BL_ONTOP,
		trans_box);
	g_signal_connect(GTK_OBJECT(button), "clicked",
		G_CALLBACK(set_blist_trans),
		(gpointer) OPT_WINTRANS_BL_ONTOP);
	gtk_box_pack_start(GTK_BOX(bltransbox), trans_box, FALSE, FALSE, 5);
	/* IM transparency slider */
	hbox = gtk_hbox_new(FALSE, 5);
	label = gtk_label_new(_("Opacity:"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	slider = gtk_hscale_new_with_range(50, 255, 1);
	gtk_range_set_value(GTK_RANGE(slider),
		purple_prefs_get_int(OPT_WINTRANS_BL_ALPHA));
	gtk_widget_set_usize(GTK_WIDGET(slider), 200, -1);
	g_signal_connect(GTK_OBJECT(slider), "value-changed",
		G_CALLBACK(bl_alpha_change), NULL);
	g_signal_connect(GTK_OBJECT(slider), "focus-out-event",
		G_CALLBACK(alpha_pref_set_int),
		(gpointer) OPT_WINTRANS_BL_ALPHA);
	gtk_box_pack_start(GTK_BOX(hbox), slider, FALSE, TRUE, 5);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(trans_box), hbox, FALSE, FALSE, 5);
	gtk_widget_show_all(ret);
	return ret;
}
