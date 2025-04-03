static gboolean
plugin_unload(PurplePlugin *plugin)
{
	if (console)
		gtk_widget_destroy(console->window);
	return TRUE;
}
