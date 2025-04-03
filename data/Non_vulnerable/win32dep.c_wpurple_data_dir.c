}
const char *wpurple_data_dir(void) {
	if (!app_data_dir) {
		/* Set app data dir, used by purple_home_dir */
		const char *newenv = g_getenv("PURPLEHOME");
		if (newenv)
			app_data_dir = g_strdup(newenv);
		else {
			app_data_dir = wpurple_get_special_folder(CSIDL_APPDATA);
			if (!app_data_dir)
				app_data_dir = g_strdup("C:");
		}
		purple_debug_info("wpurple", "Purple settings dir: %s\n",
			app_data_dir);
	}
	return app_data_dir;
}
}
const char *wpurple_data_dir(void) {
	if (!app_data_dir) {
		/* Set app data dir, used by purple_home_dir */
		const char *newenv = g_getenv("PURPLEHOME");
		if (newenv)
			app_data_dir = g_strdup(newenv);
		else {
			app_data_dir = wpurple_get_special_folder(CSIDL_APPDATA);
			if (!app_data_dir)
				app_data_dir = g_strdup("C:");
		}
		purple_debug_info("wpurple", "Purple settings dir: %s\n",
			app_data_dir);
	}
	return app_data_dir;
}
