static void
theme_font_select_face(GtkWidget *widget, gpointer prop)
{
	GtkWidget *dialog;
	PidginBlistTheme *theme;
	PidginThemeFont *font = NULL;
	const char *face;
	theme = pidgin_blist_get_theme();
	g_object_get(G_OBJECT(theme), prop, &font, NULL);
	if (!font) {
		font = pidgin_theme_font_new(NULL, NULL);
		g_object_set(G_OBJECT(theme), prop, font, NULL);
		pidgin_theme_font_free(font);
		g_object_get(G_OBJECT(theme), prop, &font, NULL);
	}
	face = pidgin_theme_font_get_font_face(font);
	dialog = gtk_font_selection_dialog_new(_("Select Font"));
	if (face && *face)
		gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(dialog),
				face);
	g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(theme_font_face_selected),
			font);
	gtk_widget_show_all(dialog);
}
