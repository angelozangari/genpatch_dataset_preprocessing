static void
config_destroy(GtkObject *w, gpointer nul)
{
	purple_debug_info("vvconfig", "closing vv configuration window\n");
	window = NULL;
}
