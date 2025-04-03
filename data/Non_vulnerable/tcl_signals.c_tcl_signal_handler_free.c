}
void tcl_signal_handler_free(struct tcl_signal_handler *handler)
{
	if (handler == NULL)
		return;
	Tcl_DecrRefCount(handler->signal);
	if (handler->namespace)
	{
		Tcl_DecrRefCount(handler->namespace);
	}
	g_free(handler);
}
