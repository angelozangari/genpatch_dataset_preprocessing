}
static void on_entry_changed(GtkEditable *editable, gpointer data)
{
	gtk_widget_set_sensitive((GtkWidget*)data,
		non_empty(gtk_entry_get_text(GTK_ENTRY(bad_entry))) &&
		non_empty(gtk_entry_get_text(GTK_ENTRY(good_entry))));
}
