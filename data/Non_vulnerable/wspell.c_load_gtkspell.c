#define GTKSPELL_DLL "libgtkspell-0.dll"
static void load_gtkspell() {
	UINT old_error_mode = SetErrorMode(SEM_FAILCRITICALERRORS);
	gchar *tmp, *tmp2;
	const char *path = g_getenv("PATH");
	tmp = g_build_filename(wpurple_install_dir(), "spellcheck", NULL);
	tmp2 = g_strdup_printf("%s%s%s", tmp,
		(path ? G_SEARCHPATH_SEPARATOR_S : ""),
		(path ? path : ""));
	g_free(tmp);
	g_setenv("PATH", tmp2, TRUE);
	g_free(tmp2);
	tmp = g_build_filename(wpurple_install_dir(), "spellcheck", GTKSPELL_DLL, NULL);
	/* Suppress error popups */
	wpidginspell_new_attach_proxy = (void*) wpurple_find_and_loadproc(tmp, "gtkspell_new_attach" );
	if (wpidginspell_new_attach_proxy) {
		wpidginspell_get_from_text_view = (void*) wpurple_find_and_loadproc(tmp, "gtkspell_get_from_text_view");
		wpidginspell_detach = (void*) wpurple_find_and_loadproc(tmp, "gtkspell_detach");
		wpidginspell_set_language = (void*) wpurple_find_and_loadproc(tmp, "gtkspell_set_language");
		wpidginspell_recheck_all = (void*) wpurple_find_and_loadproc(tmp, "gtkspell_recheck_all");
	} else {
		purple_debug_warning("wspell", "Couldn't load gtkspell (%s) \n", tmp);
		/*wpidginspell_new_attach = wgtkspell_new_attach;*/
	}
	g_free(tmp);
	SetErrorMode(old_error_mode);
}
