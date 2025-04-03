static GtkWidget *
pidgin_theme_create_font_selector(const char *text, const char *blurb, const char *prop,
		GtkSizeGroup *sizegroup)
{
	GtkWidget *color, *font;
	GtkWidget *hbox, *label;
	hbox = gtk_hbox_new(FALSE, PIDGIN_HIG_CAT_SPACE);
	label = gtk_label_new(_(text));
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_size_group_add_widget(sizegroup, label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(2, 12, 0)
	gtk_widget_set_tooltip_text(label, blurb);
#endif
	font = pidgin_pixbuf_button_from_stock("", GTK_STOCK_SELECT_FONT,
			PIDGIN_BUTTON_HORIZONTAL);
	g_signal_connect(G_OBJECT(font), "clicked", G_CALLBACK(theme_font_select_face),
			(gpointer)prop);
	gtk_box_pack_start(GTK_BOX(hbox), font, FALSE, FALSE, 0);
	color = pidgin_pixbuf_button_from_stock("", GTK_STOCK_SELECT_COLOR,
			PIDGIN_BUTTON_HORIZONTAL);
	g_signal_connect(G_OBJECT(color), "clicked", G_CALLBACK(theme_color_select),
			(gpointer)prop);
	gtk_box_pack_start(GTK_BOX(hbox), color, FALSE, FALSE, 0);
	return hbox;
}
