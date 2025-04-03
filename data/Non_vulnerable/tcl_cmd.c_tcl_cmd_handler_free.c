}
void tcl_cmd_handler_free(struct tcl_cmd_handler *handler)
{
	if (handler == NULL)
		return;
	Tcl_DecrRefCount(handler->namespace);
	g_free(handler);
}
