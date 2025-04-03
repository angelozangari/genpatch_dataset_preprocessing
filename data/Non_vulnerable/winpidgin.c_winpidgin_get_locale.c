*/
static const wchar_t *winpidgin_get_locale() {
	const wchar_t *locale = NULL;
	LCID lcid;
	wchar_t data[10];
	DWORD datalen = sizeof(data) / sizeof(wchar_t);
	/* Check if user set PIDGINLANG env var */
	if ((locale = _wgetenv(L"PIDGINLANG")))
		return locale;
	if (!portable_mode && read_reg_string(HKEY_CURRENT_USER, L"SOFTWARE\\pidgin",
			L"Installer Language", (LPBYTE) &data, &datalen)) {
		if ((locale = winpidgin_lcid_to_posix(_wtoi(data))))
			return locale;
	}
	lcid = GetUserDefaultLCID();
	if ((locale = winpidgin_lcid_to_posix(lcid)))
		return locale;
	return L"en";
}
