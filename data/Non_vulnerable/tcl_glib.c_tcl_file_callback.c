}
static gboolean tcl_file_callback(GIOChannel *source, GIOCondition condition, gpointer data)
{
	struct tcl_file_handler *tfh = data;
	struct tcl_file_event *fev;
	int mask = 0;
	if (condition & G_IO_IN)
		mask |= TCL_READABLE;
	if (condition & G_IO_OUT)
		mask |= TCL_WRITABLE;
	if (condition & (G_IO_ERR|G_IO_HUP|G_IO_NVAL))
		mask |= TCL_EXCEPTION;
	if (!(tfh->mask & (mask & ~tfh->pending)))
		return TRUE;
	tfh->pending |= mask;
	fev = (struct tcl_file_event *)ckalloc(sizeof(struct tcl_file_event));
	memset(fev, 0, sizeof(struct tcl_file_event));
	fev->header.proc = tcl_file_event_callback;
	fev->fd = tfh->fd;
	Tcl_QueueEvent((Tcl_Event *)fev, TCL_QUEUE_TAIL);
	Tcl_ServiceAll();
	return TRUE;
}
