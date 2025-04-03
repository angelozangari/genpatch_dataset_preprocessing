}
gboolean wpurple_read_reg_dword(HKEY rootkey, const char *subkey, const char *valname, LPDWORD result) {
	DWORD type;
	DWORD nbytes;
	HKEY reg_key = _reg_open_key(rootkey, subkey, KEY_QUERY_VALUE);
	gboolean success = FALSE;
	if(reg_key) {
		if(_reg_read(reg_key, valname, &type, (LPBYTE)result, &nbytes))
			success = TRUE;
		RegCloseKey(reg_key);
	}
	return success;
}
