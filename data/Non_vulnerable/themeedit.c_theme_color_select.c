static void
theme_color_select(GtkWidget *widget, gpointer prop)
{
	GtkWidget *dialog;
	PidginBlistTheme *theme;
	const GdkColor *color = NULL;
	theme = pidgin_blist_get_theme();
	if (prop_type_is_color(theme, prop)) {
		g_object_get(G_OBJECT(theme), prop, &color, NULL);
	} else {
		PidginThemeFont *pair = NULL;
		g_object_get(G_OBJECT(theme), prop, &pair, NULL);
		if (pair)
			color = pidgin_theme_font_get_color(pair);
	}
	dialog = gtk_color_selection_dialog_new(_("Select Color"));
#if GTK_CHECK_VERSION(2,14,0)
	if (color)
		gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(
			gtk_color_selection_dialog_get_color_selection(GTK_COLOR_SELECTION_DIALOG(dialog))),
			color);
#else
	if (color)
		gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog)->colorsel),
				color);
#endif
	g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(theme_color_selected),
			prop);
	gtk_widget_show_all(dialog);
}
