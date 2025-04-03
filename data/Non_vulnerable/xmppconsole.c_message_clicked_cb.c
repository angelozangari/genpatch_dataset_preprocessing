}
static void message_clicked_cb(GtkWidget *w, gpointer nul)
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *to_entry;
	GtkWidget *body_entry;
	GtkWidget *thread_entry;
	GtkWidget *subject_entry;
	GtkWidget *label;
	GtkWidget *type_combo;
	GtkSizeGroup *sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	GtkTextIter iter;
	GtkTextBuffer *buffer;
	const char *to, *body, *thread, *subject;
	char *stanza;
	int result;
	GtkWidget *dialog = gtk_dialog_new_with_buttons("<message/>",
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
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "chat");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "headline");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "groupchat");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "normal");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "error");
	gtk_combo_box_set_active(GTK_COMBO_BOX(type_combo), 0);
	gtk_box_pack_start(GTK_BOX(hbox), type_combo, FALSE, FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new("Body:");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_size_group_add_widget(sg, label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	body_entry = gtk_entry_new();
	gtk_entry_set_activates_default (GTK_ENTRY (body_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(hbox), body_entry, FALSE, FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new("Subject:");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_size_group_add_widget(sg, label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	subject_entry = gtk_entry_new();
	gtk_entry_set_activates_default (GTK_ENTRY (subject_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(hbox), subject_entry, FALSE, FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new("Thread:");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_size_group_add_widget(sg, label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	thread_entry = gtk_entry_new();
	gtk_entry_set_activates_default (GTK_ENTRY (thread_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(hbox), thread_entry, FALSE, FALSE, 0);
	gtk_widget_show_all(vbox);
	result = gtk_dialog_run(GTK_DIALOG(dialog));
	if (result != GTK_RESPONSE_ACCEPT) {
		gtk_widget_destroy(dialog);
		return;
	}
	to = gtk_entry_get_text(GTK_ENTRY(to_entry));
	body = gtk_entry_get_text(GTK_ENTRY(body_entry));
	thread = gtk_entry_get_text(GTK_ENTRY(thread_entry));
	subject = gtk_entry_get_text(GTK_ENTRY(subject_entry));
	stanza = g_strdup_printf("<message %s%s%s id='console%x' type='%s'>"
	                         "%s%s%s%s%s%s%s%s%s"
	                         "</message>",
	                         *to ? "to='" : "",
	                         *to ? to : "",
	                         *to ? "'" : "",
	                         g_random_int(),
	                         gtk_combo_box_get_active_text(GTK_COMBO_BOX(type_combo)),
	                         *body ? "<body>" : "",
	                         *body ? body : "",
	                         *body ? "</body>" : "",
	                         *subject ? "<subject>" : "",
	                         *subject ? subject : "",
	                         *subject ? "</subject>" : "",
	                         *thread ? "<thread>" : "",
	                         *thread ? thread : "",
	                         *thread ? "</thread>" : "");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console->entry));
	gtk_text_buffer_set_text(buffer, stanza, -1);
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, strstr(stanza, "</message>") - stanza);
	gtk_text_buffer_place_cursor(buffer, &iter);
	g_free(stanza);
	gtk_widget_destroy(dialog);
	g_object_unref(sg);
}
