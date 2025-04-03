}
static gboolean call_callback(gpointer data) {
	StunCallback cb = data;
	cb(&nattype);
	return FALSE;
}
