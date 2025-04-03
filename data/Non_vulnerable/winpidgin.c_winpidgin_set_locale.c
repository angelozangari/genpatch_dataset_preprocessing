}
static void winpidgin_set_locale() {
	const wchar_t *locale;
	wchar_t envstr[25];
	locale = winpidgin_get_locale();
	_snwprintf(envstr, sizeof(envstr) / sizeof(wchar_t), L"LANG=%s", locale);
	wprintf(L"Setting locale: %s\n", envstr);
	_wputenv(envstr);
}
