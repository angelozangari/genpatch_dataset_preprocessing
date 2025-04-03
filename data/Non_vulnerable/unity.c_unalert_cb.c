static int
unalert_cb(GtkWidget *widget, gpointer data, PurpleConversation *conv)
{
	unalert(conv);
	return 0;
}
