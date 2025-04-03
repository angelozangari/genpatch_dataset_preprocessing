void *
purple_status_get_handle(void) {
	static int handle;
	return &handle;
}
