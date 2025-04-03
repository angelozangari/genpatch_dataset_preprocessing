static void
winprefs_set_multiple_instances(GtkWidget *w) {
	wpurple_write_reg_string(HKEY_CURRENT_USER, "Environment", "PIDGIN_MULTI_INST",
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)) ? "1" : NULL);
}
