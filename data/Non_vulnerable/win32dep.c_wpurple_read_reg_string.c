}
char *wpurple_read_reg_string(HKEY rootkey, const char *subkey, const char *valname) {
	DWORD type;
	DWORD nbytes;
	HKEY reg_key = _reg_open_key(rootkey, subkey, KEY_QUERY_VALUE);
	char *result = NULL;
	if(reg_key) {
		if(_reg_read(reg_key, valname, &type, NULL, &nbytes) && type == REG_SZ) {
			LPBYTE data = (LPBYTE) g_new(wchar_t, ((nbytes + 1) / sizeof(wchar_t)) + 1);
			if(_reg_read(reg_key, valname, &type, data, &nbytes)) {
				wchar_t *wc_temp = (wchar_t*) data;
				wc_temp[nbytes / sizeof(wchar_t)] = '\0';
				result = g_utf16_to_utf8(wc_temp, -1,
					NULL, NULL, NULL);
			}
			g_free(data);
		}
		RegCloseKey(reg_key);
	}
	return result;
}
