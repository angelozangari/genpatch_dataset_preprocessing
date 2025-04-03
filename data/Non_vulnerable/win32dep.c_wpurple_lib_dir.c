}
const char *wpurple_lib_dir(void) {
	static gboolean initialized = FALSE;
	if (!initialized) {
		const char *inst_dir = wpurple_install_dir();
		if (inst_dir != NULL) {
			lib_dir = g_strdup_printf("%s" G_DIR_SEPARATOR_S "plugins", inst_dir);
			initialized = TRUE;
		} else {
			return NULL;
		}
	}
	return lib_dir;
}
