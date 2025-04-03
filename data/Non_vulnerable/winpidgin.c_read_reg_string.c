}
static BOOL read_reg_string(HKEY key, wchar_t *sub_key, wchar_t *val_name, LPBYTE data, LPDWORD data_len) {
	HKEY hkey;
	BOOL ret = FALSE;
	LONG retv;
	if (ERROR_SUCCESS == (retv = RegOpenKeyExW(key, sub_key, 0,
					KEY_QUERY_VALUE, &hkey))) {
		if (ERROR_SUCCESS == (retv = RegQueryValueExW(hkey, val_name,
						NULL, NULL, data, data_len)))
			ret = TRUE;
		else {
			const wchar_t *err_msg = get_win32_error_message(retv);
			wprintf(L"Could not read reg key '%s' subkey '%s' value: '%s'.\nMessage: (%ld) %s\n",
					(key == HKEY_LOCAL_MACHINE) ? L"HKLM"
					 : ((key == HKEY_CURRENT_USER) ? L"HKCU" : L"???"),
					sub_key, val_name, retv, err_msg);
		}
		RegCloseKey(hkey);
	}
	else {
		wchar_t szBuf[80];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, retv, 0,
				(LPWSTR) &szBuf, sizeof(szBuf) / sizeof(wchar_t), NULL);
		wprintf(L"Could not open reg subkey: %s\nError: (%ld) %s\n",
				sub_key, retv, szBuf);
	}
	return ret;
}
