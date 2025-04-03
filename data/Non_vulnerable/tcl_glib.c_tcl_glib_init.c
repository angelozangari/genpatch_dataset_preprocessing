}
void tcl_glib_init ()
{
	Tcl_NotifierProcs notifier;
	memset(&notifier, 0, sizeof(notifier));
	notifier.createFileHandlerProc = tcl_create_file_handler;
	notifier.deleteFileHandlerProc = tcl_delete_file_handler;
	notifier.setTimerProc = tcl_set_timer;
	notifier.waitForEventProc = tcl_wait_for_event;
	Tcl_SetNotifier(&notifier);
	Tcl_SetServiceMode(TCL_SERVICE_ALL);
	tcl_timer_pending = FALSE;
	tcl_file_handlers = g_hash_table_new(g_direct_hash, g_direct_equal);
}
