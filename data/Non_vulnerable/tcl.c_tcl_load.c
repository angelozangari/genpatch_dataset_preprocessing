}
static gboolean tcl_load(PurplePlugin *plugin)
{
	if(!tcl_loaded)
		return FALSE;
	tcl_glib_init();
	tcl_cmd_init();
	tcl_signal_init();
	purple_tcl_ref_init();
	PurpleTclRefAccount = purple_stringref_new("Account");
	PurpleTclRefConnection = purple_stringref_new("Connection");
	PurpleTclRefConversation = purple_stringref_new("Conversation");
	PurpleTclRefPointer = purple_stringref_new("Pointer");
	PurpleTclRefPlugin = purple_stringref_new("Plugin");
	PurpleTclRefPresence = purple_stringref_new("Presence");
	PurpleTclRefStatus = purple_stringref_new("Status");
	PurpleTclRefStatusAttr = purple_stringref_new("StatusAttr");
	PurpleTclRefStatusType = purple_stringref_new("StatusType");
	PurpleTclRefXfer = purple_stringref_new("Xfer");
	PurpleTclRefHandle = purple_stringref_new("Handle");
	tcl_plugins = g_hash_table_new(g_direct_hash, g_direct_equal);
#ifdef HAVE_TK
	Tcl_StaticPackage(NULL, "Tk", Tk_Init, Tk_SafeInit);
#endif /* HAVE_TK */
	return TRUE;
}
