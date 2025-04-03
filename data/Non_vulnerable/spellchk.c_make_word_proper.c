static gchar *
make_word_proper(const gchar *word)
{
	char buf[7];
	gchar *lower = g_utf8_strdown(word, -1);
	gint bytes;
	gchar *ret;
	bytes = g_unichar_to_utf8(g_unichar_toupper(g_utf8_get_char(word)), buf);
	g_assert(bytes >= 0);
	buf[MIN((gsize)bytes, sizeof(buf) - 1)] = '\0';
	ret = g_strconcat(buf, g_utf8_offset_to_pointer(lower, 1), NULL);
	g_free(lower);
	return ret;
}
