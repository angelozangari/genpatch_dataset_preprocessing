}
static gboolean tcl_unload(PurplePlugin *plugin)
{
	g_hash_table_destroy(tcl_plugins);
	tcl_plugins = NULL;
	purple_stringref_unref(PurpleTclRefAccount);
	purple_stringref_unref(PurpleTclRefConnection);
	purple_stringref_unref(PurpleTclRefConversation);
	purple_stringref_unref(PurpleTclRefPointer);
	purple_stringref_unref(PurpleTclRefPlugin);
	purple_stringref_unref(PurpleTclRefPresence);
	purple_stringref_unref(PurpleTclRefStatus);
	purple_stringref_unref(PurpleTclRefStatusAttr);
	purple_stringref_unref(PurpleTclRefStatusType);
	purple_stringref_unref(PurpleTclRefXfer);
	return TRUE;
}
