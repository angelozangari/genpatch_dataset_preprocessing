}
static HKEY _reg_open_key(HKEY rootkey, const char *subkey, REGSAM access) {
	HKEY reg_key = NULL;
	LONG rv;
	wchar_t *wc_subkey = g_utf8_to_utf16(subkey, -1, NULL,
		NULL, NULL);
	rv = RegOpenKeyExW(rootkey, wc_subkey, 0, access, &reg_key);
	g_free(wc_subkey);
	if (rv != ERROR_SUCCESS) {
		char *errmsg = g_win32_error_message(rv);
		purple_debug_error("wpurple", "Could not open reg key '%s' subkey '%s'.\nMessage: (%ld) %s\n",
					((rootkey == HKEY_LOCAL_MACHINE) ? "HKLM" :
					 (rootkey == HKEY_CURRENT_USER) ? "HKCU" :
					  (rootkey == HKEY_CLASSES_ROOT) ? "HKCR" : "???"),
					subkey, rv, errmsg);
		g_free(errmsg);
	}
	return reg_key;
}
