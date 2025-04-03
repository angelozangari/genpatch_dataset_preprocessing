}
static void iq_clicked_cb(GtkWidget *w, gpointer nul)
{
	GtkWidget *vbox, *hbox, *to_entry, *label, *type_combo;
	GtkSizeGroup *sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	GtkTextIter iter;
	GtkTextBuffer *buffer;
	const char *to;
	int result;
	char *stanza;
	GtkWidget *dialog = gtk_dialog_new_with_buttons("<iq/>",
							GTK_WINDOW(console->window),
							GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_STOCK_CANCEL,
							GTK_RESPONSE_REJECT,
							GTK_STOCK_OK,
							GTK_RESPONSE_ACCEPT,
							NULL);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
	gtk_dialog_set_default_response (GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 12);
#if GTK_CHECK_VERSION(2,14,0)
	vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
#else
	vbox = GTK_DIALOG(dialog)->vbox;
#endif
	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new("To:");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_size_group_add_widget(sg, label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	to_entry = gtk_entry_new();
	gtk_entry_set_activates_default (GTK_ENTRY (to_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(hbox), to_entry, FALSE, FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new("Type:");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_size_group_add_widget(sg, label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	type_combo = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "get");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "set");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "result");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "error");
	gtk_combo_box_set_active(GTK_COMBO_BOX(type_combo), 0);
	gtk_box_pack_start(GTK_BOX(hbox), type_combo, FALSE, FALSE, 0);
	gtk_widget_show_all(vbox);
	result = gtk_dialog_run(GTK_DIALOG(dialog));
	if (result != GTK_RESPONSE_ACCEPT) {
		gtk_widget_destroy(dialog);
		return;
	}
	to = gtk_entry_get_text(GTK_ENTRY(to_entry));
	stanza = g_strdup_printf("<iq %s%s%s id='console%x' type='%s'></iq>",
				 to && *to ? "to='" : "",
				 to && *to ? to : "",
				 to && *to ? "'" : "",
				 g_random_int(),
				 gtk_combo_box_get_active_text(GTK_COMBO_BOX(type_combo)));
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console->entry));
	gtk_text_buffer_set_text(buffer, stanza, -1);
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, strstr(stanza, "</iq>") - stanza);
	gtk_text_buffer_place_cursor(buffer, &iter);
	g_free(stanza);
	gtk_widget_destroy(dialog);
	g_object_unref(sg);
}
