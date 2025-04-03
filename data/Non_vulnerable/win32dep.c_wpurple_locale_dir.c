}
const char *wpurple_locale_dir(void) {
	static gboolean initialized = FALSE;
	if (!initialized) {
		const char *inst_dir = wpurple_install_dir();
		if (inst_dir != NULL) {
			locale_dir = g_strdup_printf("%s" G_DIR_SEPARATOR_S "locale", inst_dir);
			initialized = TRUE;
		} else {
			return NULL;
		}
	}
	return locale_dir;
}
