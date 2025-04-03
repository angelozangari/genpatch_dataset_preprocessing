static gboolean
plugin_unload(PurplePlugin *plugin)
{
	g_signal_remove_emission_hook(signal_id, hook_id);
	return TRUE;
}
