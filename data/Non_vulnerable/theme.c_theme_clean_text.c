static gchar *
theme_clean_text(const gchar *text)
{
	gchar *clean_text = NULL;
	if (text != NULL) {
		clean_text = g_markup_escape_text(text, -1);
		g_strdelimit(clean_text, "\n", ' ');
		purple_str_strip_char(clean_text, '\r');
	}
	return clean_text;
}
