}
static void tcl_set_timer(CONST86 Tcl_Time *timePtr)
{
	guint interval;
	if (tcl_timer_pending)
		g_source_remove(tcl_timer);
	if (timePtr == NULL) {
		tcl_timer_pending = FALSE;
		return;
	}
	interval = timePtr->sec * 1000 + (timePtr->usec ? timePtr->usec / 1000 : 0);
	tcl_timer = g_timeout_add(interval, tcl_kick, NULL);
	tcl_timer_pending = TRUE;
}
