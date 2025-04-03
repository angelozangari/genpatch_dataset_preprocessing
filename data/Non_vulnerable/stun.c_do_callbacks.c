}
static void do_callbacks(void) {
	while (callbacks) {
		StunCallback cb = callbacks->data;
		if (cb)
			cb(&nattype);
		callbacks = g_slist_delete_link(callbacks, callbacks);
	}
}
}
static void do_callbacks(void) {
	while (callbacks) {
		StunCallback cb = callbacks->data;
		if (cb)
			cb(&nattype);
		callbacks = g_slist_delete_link(callbacks, callbacks);
	}
}
