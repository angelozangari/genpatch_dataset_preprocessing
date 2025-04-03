}
static void presence_clicked_cb(GtkWidget *w, gpointer nul)
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *to_entry;
	GtkWidget *status_entry;
	GtkWidget *priority_entry;
	GtkWidget *label;
	GtkWidget *show_combo;
	GtkWidget *type_combo;
	GtkSizeGroup *sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	GtkTextIter iter;
	GtkTextBuffer *buffer;
	const char *to, *type, *status, *show, *priority;
	int result;
	char *stanza;
	GtkWidget *dialog = gtk_dialog_new_with_buttons("<presence/>",
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
	gtk_size_group_add_widget(sg, label);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
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
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "default");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "unavailable");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "subscribe");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "unsubscribe");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "subscribed");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "unsubscribed");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "probe");
	gtk_combo_box_append_text(GTK_COMBO_BOX(type_combo), "error");
	gtk_combo_box_set_active(GTK_COMBO_BOX(type_combo), 0);
	gtk_box_pack_start(GTK_BOX(hbox), type_combo, FALSE, FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new("Show:");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_size_group_add_widget(sg, label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	show_combo = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(show_combo), "default");
	gtk_combo_box_append_text(GTK_COMBO_BOX(show_combo), "away");
	gtk_combo_box_append_text(GTK_COMBO_BOX(show_combo), "dnd");
	gtk_combo_box_append_text(GTK_COMBO_BOX(show_combo), "xa");
	gtk_combo_box_append_text(GTK_COMBO_BOX(show_combo), "chat");
	gtk_combo_box_set_active(GTK_COMBO_BOX(show_combo), 0);
	gtk_box_pack_start(GTK_BOX(hbox), show_combo, FALSE, FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new("Status:");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_size_group_add_widget(sg, label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	status_entry = gtk_entry_new();
	gtk_entry_set_activates_default (GTK_ENTRY (status_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(hbox), status_entry, FALSE, FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new("Priority:");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_size_group_add_widget(sg, label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	priority_entry = gtk_spin_button_new_with_range(-128, 127, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(priority_entry), 0);
	gtk_box_pack_start(GTK_BOX(hbox), priority_entry, FALSE, FALSE, 0);
	gtk_widget_show_all(vbox);
	result = gtk_dialog_run(GTK_DIALOG(dialog));
	if (result != GTK_RESPONSE_ACCEPT) {
		gtk_widget_destroy(dialog);
		return;
	}
	to = gtk_entry_get_text(GTK_ENTRY(to_entry));
	type = gtk_combo_box_get_active_text(GTK_COMBO_BOX(type_combo));
	if (!strcmp(type, "default"))
		type = "";
	show = gtk_combo_box_get_active_text(GTK_COMBO_BOX(show_combo));
	if (!strcmp(show, "default"))
		show = "";
	status = gtk_entry_get_text(GTK_ENTRY(status_entry));
	priority = gtk_entry_get_text(GTK_ENTRY(priority_entry));
	if (!strcmp(priority, "0"))
		priority = "";
	stanza = g_strdup_printf("<presence %s%s%s id='console%x' %s%s%s>"
	                         "%s%s%s%s%s%s%s%s%s"
	                         "</presence>",
	                         *to ? "to='" : "",
	                         *to ? to : "",
	                         *to ? "'" : "",
	                         g_random_int(),
	                         *type ? "type='" : "",
	                         *type ? type : "",
	                         *type ? "'" : "",
	                         *show ? "<show>" : "",
	                         *show ? show : "",
	                         *show ? "</show>" : "",
	                         *status ? "<status>" : "",
	                         *status ? status : "",
	                         *status ? "</status>" : "",
	                         *priority ? "<priority>" : "",
	                         *priority ? priority : "",
	                         *priority ? "</priority>" : "");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console->entry));
	gtk_text_buffer_set_text(buffer, stanza, -1);
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, strstr(stanza, "</presence>") - stanza);
	gtk_text_buffer_place_cursor(buffer, &iter);
	g_free(stanza);
	gtk_widget_destroy(dialog);
	g_object_unref(sg);
}
