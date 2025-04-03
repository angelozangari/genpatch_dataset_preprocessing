static void
theme_color_selected(GtkDialog *dialog, gint response, const char *prop)
{
	if (response == GTK_RESPONSE_OK) {
		GtkWidget *colorsel;
		GdkColor color;
		PidginBlistTheme *theme;
#if GTK_CHECK_VERSION(2,14,0)
		colorsel =
			gtk_color_selection_dialog_get_color_selection(GTK_COLOR_SELECTION_DIALOG(dialog));
#else
		colorsel = GTK_COLOR_SELECTION_DIALOG(dialog)->colorsel;
#endif
		gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(colorsel), &color);
		theme = pidgin_blist_get_theme();
		if (prop_type_is_color(theme, prop)) {
			g_object_set(G_OBJECT(theme), prop, &color, NULL);
		} else {
			PidginThemeFont *font = NULL;
			g_object_get(G_OBJECT(theme), prop, &font, NULL);
			if (!font) {
				font = pidgin_theme_font_new(NULL, &color);
				g_object_set(G_OBJECT(theme), prop, font, NULL);
				pidgin_theme_font_free(font);
			} else {
				pidgin_theme_font_set_color(font, &color);
			}
		}
		pidgin_blist_set_theme(theme);
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}
