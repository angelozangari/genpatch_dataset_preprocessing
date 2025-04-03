static void
signing_on_cb(PurpleConnection *gc)
{
	if (!console)
		return;
	gtk_combo_box_append_text(GTK_COMBO_BOX(console->dropdown), purple_account_get_username(gc->account));
	console->accounts = g_list_append(console->accounts, gc);
	console->count++;
	if (console->count == 1)
		console->gc = gc;
	else
		gtk_widget_show_all(console->hbox);
}
