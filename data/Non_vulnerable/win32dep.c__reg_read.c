}
static gboolean _reg_read(HKEY reg_key, const char *valname, LPDWORD type, LPBYTE data, LPDWORD data_len) {
	LONG rv;
	wchar_t *wc_valname = NULL;
	if (valname)
		wc_valname = g_utf8_to_utf16(valname, -1, NULL, NULL, NULL);
	rv = RegQueryValueExW(reg_key, wc_valname, 0, type, data, data_len);
	g_free(wc_valname);
	if (rv != ERROR_SUCCESS) {
		char *errmsg = g_win32_error_message(rv);
		purple_debug_error("wpurple", "Could not read from reg key value '%s'.\nMessage: (%ld) %s\n",
					valname, rv, errmsg);
		g_free(errmsg);
	}
	return (rv == ERROR_SUCCESS);
}
