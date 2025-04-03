static void
dropdown_changed_cb(GtkComboBox *widget, gpointer nul)
{
	PurpleAccount *account;
	if (!console)
		return;
	account = purple_accounts_find(gtk_combo_box_get_active_text(GTK_COMBO_BOX(console->dropdown)),
				    "prpl-jabber");
	if (!account || !account->gc)
		return;
	console->gc = account->gc;
	gtk_imhtml_clear(GTK_IMHTML(console->imhtml));
}
