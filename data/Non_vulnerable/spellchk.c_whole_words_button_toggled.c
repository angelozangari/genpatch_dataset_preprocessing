}
static void whole_words_button_toggled(GtkToggleButton *complete_toggle, GtkToggleButton *case_toggle)
{
	gboolean enabled = gtk_toggle_button_get_active(complete_toggle);
	gtk_toggle_button_set_active(case_toggle, !enabled);
	gtk_widget_set_sensitive(GTK_WIDGET(case_toggle), enabled);
}
