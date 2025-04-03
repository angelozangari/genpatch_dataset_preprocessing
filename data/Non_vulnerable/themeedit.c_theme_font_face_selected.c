static void
theme_font_face_selected(GtkWidget *dialog, gint response, gpointer font)
{
	if (response == GTK_RESPONSE_OK || response == GTK_RESPONSE_APPLY) {
		const char *fontname = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dialog));
		pidgin_theme_font_set_font_face(font, fontname);
		pidgin_blist_refresh(purple_get_blist());
	}
	gtk_widget_destroy(dialog);
}
