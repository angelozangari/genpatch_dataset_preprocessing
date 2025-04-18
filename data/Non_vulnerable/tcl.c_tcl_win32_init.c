#define Tk_Init wtk_Init
static gboolean tcl_win32_init() {
	const char regkey[] = "SOFTWARE\\ActiveState\\ActiveTcl\\";
	char *version = NULL;
	gboolean retval = FALSE;
	if ((version = wpurple_read_reg_string(HKEY_LOCAL_MACHINE, regkey, "CurrentVersion"))
			|| (version = wpurple_read_reg_string(HKEY_CURRENT_USER, regkey, "CurrentVersion"))) {
		char *path = NULL;
		char *regkey2;
		char **tokens;
		int major = 0, minor = 0, micro = 0;
		tokens = g_strsplit(version, ".", 0);
		if (tokens[0] && tokens[1] && tokens[2]) {
			major = atoi(tokens[0]);
			minor = atoi(tokens[1]);
			micro = atoi(tokens[2]);
		}
		g_strfreev(tokens);
		regkey2 = g_strdup_printf("%s%s\\", regkey, version);
		if (!(major == 8 && minor == 4 && micro >= 5))
			purple_debug(PURPLE_DEBUG_INFO, "tcl", "Unsupported ActiveTCL version %s found.\n", version);
		else if ((path = wpurple_read_reg_string(HKEY_LOCAL_MACHINE, regkey2, NULL)) || (path = wpurple_read_reg_string(HKEY_CURRENT_USER, regkey2, NULL))) {
			char *tclpath;
			char *tkpath;
			purple_debug(PURPLE_DEBUG_INFO, "tcl", "Loading ActiveTCL version %s from \"%s\"\n", version, path);
			tclpath = g_build_filename(path, "bin", "tcl84.dll", NULL);
			tkpath = g_build_filename(path, "bin", "tk84.dll", NULL);
			if(!(wtcl_CreateInterp = (LPFNTCLCREATEINTERP) wpurple_find_and_loadproc(tclpath, "Tcl_CreateInterp"))) {
				purple_debug(PURPLE_DEBUG_INFO, "tcl", "tcl_win32_init error loading Tcl_CreateInterp\n");
			} else {
				if(!(wtk_Init = (LPFNTKINIT) wpurple_find_and_loadproc(tkpath, "Tk_Init"))) {
					HMODULE mod;
					purple_debug(PURPLE_DEBUG_INFO, "tcl", "tcl_win32_init error loading Tk_Init\n");
					if((mod = GetModuleHandle("tcl84.dll")))
						FreeLibrary(mod);
				} else {
					retval = TRUE;
				}
			}
			g_free(tclpath);
			g_free(tkpath);
		}
		g_free(path);
		g_free(regkey2);
	}
	g_free(version);
	if (!retval)
		purple_debug(PURPLE_DEBUG_INFO, "tcl", _("Unable to detect ActiveTCL installation. If you wish to use TCL plugins, install ActiveTCL from http://www.activestate.com\n"));
	return retval;
}
