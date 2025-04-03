}
const char *wpurple_install_dir(void) {
	static gboolean initialized = FALSE;
	if (!initialized) {
		char *tmp = NULL;
		wchar_t winstall_dir[MAXPATHLEN];
		if (GetModuleFileNameW(libpurpledll_hInstance, winstall_dir,
				MAXPATHLEN) > 0) {
			tmp = g_utf16_to_utf8(winstall_dir, -1,
				NULL, NULL, NULL);
		}
		if (tmp == NULL) {
			tmp = g_win32_error_message(GetLastError());
			purple_debug_error("wpurple",
				"GetModuleFileName error: %s\n", tmp);
			g_free(tmp);
			return NULL;
		} else {
			install_dir = g_path_get_dirname(tmp);
			g_free(tmp);
			initialized = TRUE;
		}
	}
	return install_dir;
}
