}
int tcl_file_event_callback(Tcl_Event *event, int flags)
{
	struct tcl_file_handler *tfh;
	struct tcl_file_event *fev = (struct tcl_file_event *)event;
	int mask;
	if (!(flags & TCL_FILE_EVENTS)) {
		return 0;
	}
	tfh = g_hash_table_lookup(tcl_file_handlers, GINT_TO_POINTER(fev->fd));
	if (tfh == NULL)
		return 1;
	mask = tfh->mask & tfh->pending;
	if (mask)
		(*tfh->proc)(tfh->data, mask);
	tfh->pending = 0;
	return 1;
}
