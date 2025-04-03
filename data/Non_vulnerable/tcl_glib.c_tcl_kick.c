}
static gboolean tcl_kick(gpointer data)
{
	tcl_timer_pending = FALSE;
	Tcl_ServiceAll();
	return FALSE;
}
