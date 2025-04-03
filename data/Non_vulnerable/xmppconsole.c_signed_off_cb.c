static void
signed_off_cb(PurpleConnection *gc)
{
	int i = 0;
	GList *l;
	if (!console)
		return;
	l = console->accounts;
	while (l) {
		PurpleConnection *g = l->data;
		if (gc == g)
			break;
		i++;
		l = l->next;
	}
	if (l == NULL)
		return;
	gtk_combo_box_remove_text(GTK_COMBO_BOX(console->dropdown), i);
	console->accounts = g_list_remove(console->accounts, gc);
	console->count--;
	if (gc == console->gc) {
		console->gc = NULL;
		gtk_imhtml_append_text(GTK_IMHTML(console->imhtml),
				       _("<font color='#777777'>Logged out.</font>"), 0);
	}
}
