static void
winprefs_set_autostart(GtkWidget *w) {
	char *runval = NULL;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
		runval = g_strdup_printf("\"%s" G_DIR_SEPARATOR_S "pidgin.exe\"", wpurple_install_dir());
	if(!wpurple_write_reg_string(HKEY_CURRENT_USER, RUNKEY, "Pidgin", runval)
		/* For Win98 */
		&& !wpurple_write_reg_string(HKEY_LOCAL_MACHINE, RUNKEY, "Pidgin", runval))
			purple_debug_error(WINPREFS_PLUGIN_ID, "Could not set registry key value\n");
	g_free(runval);
}
