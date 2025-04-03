static void
console_destroy(GtkObject *window, gpointer nul)
{
	g_list_free(console->accounts);
	g_free(console);
	console = NULL;
}
