/* GTKSPELL DUMMY FUNCS */
static GtkSpell* wgtkspell_new_attach(GtkTextView *view, const gchar *lang, GError **error) {
	GtkSpell *ret = NULL;
	if (wpidginspell_new_attach_proxy) {
		UINT old_error_mode = SetErrorMode(SEM_FAILCRITICALERRORS);
		ret = wpidginspell_new_attach_proxy(view, lang, error);
		SetErrorMode(old_error_mode);
	}
	return ret;
}
