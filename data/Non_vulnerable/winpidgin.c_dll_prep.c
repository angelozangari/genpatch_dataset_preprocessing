}
static BOOL dll_prep(const wchar_t *pidgin_dir) {
	wchar_t path[MAX_PATH + 1];
	path[0] = L'\0';
	if (*pidgin_dir) {
		_snwprintf(path, sizeof(path) / sizeof(wchar_t), L"%s\\Gtk\\bin", pidgin_dir);
		path[sizeof(path) / sizeof(wchar_t) - 1] = L'\0';
	}
	return common_dll_prep(path);
}
