}
static void tcl_create_file_handler(int fd, int mask, Tcl_FileProc *proc, ClientData data)
{
	struct tcl_file_handler *tfh = g_new0(struct tcl_file_handler, 1);
	GIOChannel *channel;
	GIOCondition cond = 0;
	if (g_hash_table_lookup(tcl_file_handlers, GINT_TO_POINTER(fd)))
            tcl_delete_file_handler(fd);
	if (mask & TCL_READABLE)
		cond |= G_IO_IN;
	if (mask & TCL_WRITABLE)
		cond |= G_IO_OUT;
	if (mask & TCL_EXCEPTION)
		cond |= G_IO_ERR|G_IO_HUP|G_IO_NVAL;
	tfh->fd = fd;
	tfh->mask = mask;
	tfh->proc = proc;
	tfh->data = data;
	channel = g_io_channel_unix_new(fd);
	tfh->source = g_io_add_watch_full(channel, G_PRIORITY_DEFAULT, cond, tcl_file_callback, tfh, g_free);
	g_io_channel_unref(channel);
	g_hash_table_insert(tcl_file_handlers, GINT_TO_POINTER(fd), tfh);
	Tcl_ServiceAll();
}
