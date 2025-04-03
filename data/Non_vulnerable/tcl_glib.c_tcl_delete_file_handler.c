}
static void tcl_delete_file_handler(int fd)
{
	struct tcl_file_handler *tfh = g_hash_table_lookup(tcl_file_handlers, GINT_TO_POINTER(fd));
	if (tfh == NULL)
		return;
	g_source_remove(tfh->source);
	g_hash_table_remove(tcl_file_handlers, GINT_TO_POINTER(fd));
	Tcl_ServiceAll();
}
