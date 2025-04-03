}
void pidgin_icon_theme_edit(PurplePluginAction *unused)
{
	GtkWidget *dialog;
	GtkWidget *box, *vbox;
	GtkWidget *notebook;
	GtkSizeGroup *sizegroup;
	int s, i, j;
	dialog = pidgin_create_dialog(_("Pidgin Icon Theme Editor"), 0, "theme-editor-icon", FALSE);
	box = pidgin_dialog_get_vbox_with_properties(GTK_DIALOG(dialog), FALSE, PIDGIN_HIG_BOX_SPACE);
	notebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(box), notebook, TRUE, TRUE, PIDGIN_HIG_BOX_SPACE);
	sizegroup = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	for (s = 0; sections[s].heading; s++) {
		const char *heading = sections[s].heading;
		box = gtk_vbox_new(FALSE, 0);
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, gtk_label_new(heading));
		vbox = pidgin_make_frame(box, heading);
		g_object_set_data(G_OBJECT(dialog), heading, vbox);
		for (i = 0; sections[s].options[i].stockid; i++) {
			const char *id = sections[s].options[i].stockid;
			const char *text = _(sections[s].options[i].text);
			GtkWidget *hbox = gtk_hbox_new(FALSE, PIDGIN_HIG_CAT_SPACE);
			GtkWidget *label = gtk_label_new(text);
			GtkWidget *image = gtk_image_new_from_stock(id,
					gtk_icon_size_from_name(PIDGIN_ICON_SIZE_TANGO_EXTRA_SMALL));
			GtkWidget *ebox = gtk_event_box_new();
			gtk_container_add(GTK_CONTAINER(ebox), image);
			gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
			g_signal_connect(G_OBJECT(ebox), "button-press-event", G_CALLBACK(change_stock_image), image);
			g_object_set_data(G_OBJECT(image), "property-name", (gpointer)id);
			g_object_set_data(G_OBJECT(image), "localized-name", (gpointer)text);
			gtk_size_group_add_widget(sizegroup, label);
			gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
			gtk_box_pack_start(GTK_BOX(hbox), ebox, FALSE, FALSE, 0);
			for (j = 0; stocksizes[j]; j++) {
				GtkWidget *sh;
				if (!(sections[s].flags & (1 << j)))
					continue;
				sh = gtk_image_new_from_stock(id, gtk_icon_size_from_name(stocksizes[j]));
				gtk_box_pack_start(GTK_BOX(hbox), sh, FALSE, FALSE, 0);
				g_object_set_data(G_OBJECT(image), stocksizes[j], sh);
			}
			gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
			g_object_set_data(G_OBJECT(vbox), id, image);
		}
	}
#ifdef NOT_SADRUL
	pidgin_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_SAVE, G_CALLBACK(save_icon_theme), dialog);
#endif
	pidgin_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_APPLY, G_CALLBACK(use_icon_theme), dialog);
	pidgin_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CLOSE, G_CALLBACK(close_icon_theme), dialog);
	gtk_widget_show_all(dialog);
	g_object_unref(sizegroup);
}
