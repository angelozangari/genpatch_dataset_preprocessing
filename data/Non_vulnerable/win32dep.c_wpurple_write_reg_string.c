/* Miscellaneous */
gboolean wpurple_write_reg_string(HKEY rootkey, const char *subkey, const char *valname,
		const char *value) {
	HKEY reg_key;
	gboolean success = FALSE;
	wchar_t *wc_subkey = g_utf8_to_utf16(subkey, -1, NULL,
		NULL, NULL);
	if(RegOpenKeyExW(rootkey, wc_subkey, 0,
			KEY_SET_VALUE, &reg_key) == ERROR_SUCCESS) {
		wchar_t *wc_valname = NULL;
		if (valname)
			wc_valname = g_utf8_to_utf16(valname, -1,
				NULL, NULL, NULL);
		if(value) {
			wchar_t *wc_value = g_utf8_to_utf16(value, -1,
				NULL, NULL, NULL);
			int len = (wcslen(wc_value) * sizeof(wchar_t)) + 1;
			if(RegSetValueExW(reg_key, wc_valname, 0, REG_SZ,
					(LPBYTE)wc_value, len
					) == ERROR_SUCCESS)
				success = TRUE;
			g_free(wc_value);
		} else
			if(RegDeleteValueW(reg_key, wc_valname) ==  ERROR_SUCCESS)
				success = TRUE;
		g_free(wc_valname);
	}
	g_free(wc_subkey);
	if(reg_key != NULL)
		RegCloseKey(reg_key);
	return success;
}
