static void
set_timestamp(GtkWidget *spinner, void *null)
{
	int tm;
	tm = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));
	purple_debug(PURPLE_DEBUG_MISC, "timestamp",
		"setting interval to %d minutes\n", tm);
	interval = tm * 60;
	purple_prefs_set_int("/plugins/gtk/timestamp/interval", interval * 1000);
}
