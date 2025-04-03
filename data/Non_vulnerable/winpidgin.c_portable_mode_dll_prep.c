}
static void portable_mode_dll_prep(const wchar_t *pidgin_dir) {
	/* need to be able to fit MAX_PATH + "PURPLEHOME=" in path2 */
	wchar_t path[MAX_PATH + 1];
	wchar_t path2[MAX_PATH + 12];
	const wchar_t *prev = NULL;
	/* We assume that GTK+ is installed under \\path\to\Pidgin\..\GTK
	 * First we find \\path\to
	 */
	if (*pidgin_dir)
		/* pidgin_dir points to \\path\to\Pidgin */
		prev = wcsrchr(pidgin_dir, L'\\');
	if (prev) {
		int cnt = (prev - pidgin_dir);
		wcsncpy(path, pidgin_dir, cnt);
		path[cnt] = L'\0';
	} else {
		printf("Unable to determine current executable path. \n"
			"This will prevent the settings dir from being set.\n"
			"Assuming GTK+ is in the PATH.\n");
		return;
	}
	/* Set $HOME so that the GTK+ settings get stored in the right place */
	_snwprintf(path2, sizeof(path2) / sizeof(wchar_t), L"HOME=%s", path);
	_wputenv(path2);
	/* Set up the settings dir base to be \\path\to
	 * The actual settings dir will be \\path\to\.purple */
	_snwprintf(path2, sizeof(path2) / sizeof(wchar_t), L"PURPLEHOME=%s", path);
	wprintf(L"Setting settings dir: %s\n", path2);
	_wputenv(path2);
	if (!dll_prep(pidgin_dir)) {
		/* set the GTK+ path to be \\path\to\GTK\bin */
		wcscat(path, L"\\GTK\\bin");
		common_dll_prep(path);
	}
}
