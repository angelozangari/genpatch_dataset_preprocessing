   If so, load it (if not already loaded). */
FARPROC wpurple_find_and_loadproc(const char *dllname, const char *procedure) {
	HMODULE hmod;
	BOOL did_load = FALSE;
	FARPROC proc = 0;
	wchar_t *wc_dllname = g_utf8_to_utf16(dllname, -1, NULL, NULL, NULL);
	if(!(hmod = GetModuleHandleW(wc_dllname))) {
		purple_debug_warning("wpurple", "%s not already loaded; loading it...\n", dllname);
		if(!(hmod = LoadLibraryW(wc_dllname))) {
			purple_debug_error("wpurple", "Could not load: %s (%s)\n", dllname,
				g_win32_error_message(GetLastError()));
			g_free(wc_dllname);
			return NULL;
		}
		else
			did_load = TRUE;
	}
	g_free(wc_dllname);
	wc_dllname = NULL;
	if((proc = GetProcAddress(hmod, procedure))) {
		purple_debug_info("wpurple", "This version of %s contains %s\n",
			dllname, procedure);
		return proc;
	}
	else {
		purple_debug_warning("wpurple", "Function %s not found in dll %s\n",
			procedure, dllname);
		if(did_load) {
			/* unload dll */
			FreeLibrary(hmod);
		}
		return NULL;
	}
}
