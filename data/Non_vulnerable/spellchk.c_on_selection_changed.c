}
static void on_selection_changed(GtkTreeSelection *sel,
	gpointer data)
{
	gint num_selected;
	num_selected = gtk_tree_selection_count_selected_rows(sel);
	gtk_widget_set_sensitive((GtkWidget*)data, (num_selected > 0));
}
