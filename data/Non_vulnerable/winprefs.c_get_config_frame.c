}
static GtkWidget* get_config_frame(PurplePlugin *plugin) {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkWidget *button;
	char *run_key_val;
	char *tmp;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width(GTK_CONTAINER(ret), 12);
	/* Autostart */
	vbox = pidgin_make_frame(ret, _("Startup"));
	tmp = g_strdup_printf(_("_Start %s on Windows startup"), PIDGIN_NAME);
	button = gtk_check_button_new_with_mnemonic(tmp);
	g_free(tmp);
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	if ((run_key_val = wpurple_read_reg_string(HKEY_CURRENT_USER, RUNKEY, "Pidgin"))
			|| (run_key_val = wpurple_read_reg_string(HKEY_LOCAL_MACHINE, RUNKEY, "Pidgin"))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
		g_free(run_key_val);
	}
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(winprefs_set_autostart), NULL);
	gtk_widget_show(button);
	button = gtk_check_button_new_with_mnemonic(_("Allow multiple instances"));
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	if ((run_key_val = wpurple_read_reg_string(HKEY_CURRENT_USER, "Environment", "PIDGIN_MULTI_INST"))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
		g_free(run_key_val);
	}
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(winprefs_set_multiple_instances), NULL);
	gtk_widget_show(button);
	/* Buddy List */
	vbox = pidgin_make_frame(ret, _("Buddy List"));
	pidgin_prefs_checkbox(_("_Dockable Buddy List"),
							PREF_DBLIST_DOCKABLE, vbox);
	/* Blist On Top */
	pidgin_prefs_dropdown(vbox, _("_Keep Buddy List window on top:"),
		PURPLE_PREF_INT, PREF_BLIST_ON_TOP,
		_("Never"), BLIST_TOP_NEVER,
		_("Always"), BLIST_TOP_ALWAYS,
		/* XXX: Did this ever work? */
		_("Only when docked"), BLIST_TOP_DOCKED,
		NULL);
	gtk_widget_show_all(ret);
	return ret;
}
