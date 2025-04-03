static GtkWidget *
get_config_frame(PurplePlugin *plugin)
{
	GtkWidget *ret;
	GtkWidget *frame, *label;
	GtkWidget *vbox, *hbox;
	GtkObject *adj;
	GtkWidget *spinner;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);
	frame = pidgin_make_frame(ret, _("Display Timestamps Every"));
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
	/* XXX limit to divisors of 60? */
	adj = gtk_adjustment_new(interval / 60, 1, 60, 1, 0, 0);
	spinner = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(spinner), "value-changed",
		G_CALLBACK(set_timestamp), NULL);
	label = gtk_label_new(_("minutes"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_widget_show_all(ret);
	return ret;
}
