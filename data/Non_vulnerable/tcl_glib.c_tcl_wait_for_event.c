}
static int tcl_wait_for_event(CONST86 Tcl_Time *timePtr)
{
	if (!timePtr || (timePtr->sec == 0 && timePtr->usec == 0)) {
		g_main_context_iteration(NULL, FALSE);
		return 1;
	} else {
		tcl_set_timer(timePtr);
	}
	g_main_context_iteration(NULL, TRUE);
	return 1;
}
