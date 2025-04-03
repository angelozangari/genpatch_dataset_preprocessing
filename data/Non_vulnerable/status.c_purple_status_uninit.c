void
purple_status_uninit(void)
{
	purple_prefs_disconnect_by_handle(purple_prefs_get_handle());
}
