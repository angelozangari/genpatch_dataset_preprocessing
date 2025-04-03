static gboolean
plugin_unload(PurplePlugin *plugin)
{
	GList *convs = purple_get_conversations();
	while (convs) {
		PurpleConversation *conv = (PurpleConversation *)convs->data;
		unalert(conv);
		detach_signals(conv);
		convs = convs->next;
	}
	unity_launcher_entry_set_count_visible(launcher, FALSE);
	messaging_menu_app_unregister(mmapp);
	g_object_unref(launcher);
	g_object_unref(mmapp);
	return TRUE;
}
