static gboolean
spellchk_inside_word(GtkTextIter *iter)
{
	gunichar ucs4_char = gtk_text_iter_get_char(iter);
	gchar *utf8_str;
	gchar c = 0;
	utf8_str = g_ucs4_to_utf8(&ucs4_char, 1, NULL, NULL, NULL);
	if (utf8_str != NULL)
	{
		c = utf8_str[0];
		g_free(utf8_str);
	}
	/* Hack because otherwise typing things like U.S. gets difficult
	 * if you have 'u' -> 'you' set as a correction...
	 *
	 * Part 1 of 2: This marks . as being an inside-word character. */
	if (c == '.')
		return TRUE;
	/* Avoid problems with \r, for example (SF #1289031). */
	if (c == '\\')
		return TRUE;
	if (gtk_text_iter_inside_word (iter) == TRUE)
		return TRUE;
	if (c == '\'') {
		gboolean result = gtk_text_iter_backward_char(iter);
		gboolean output = gtk_text_iter_inside_word(iter);
		if (result)
		{
			/*
			 * Hack so that "u'll" will correct correctly.
			 */
			ucs4_char = gtk_text_iter_get_char(iter);
			utf8_str = g_ucs4_to_utf8(&ucs4_char, 1, NULL, NULL, NULL);
			if (utf8_str != NULL)
			{
				c = utf8_str[0];
				g_free(utf8_str);
				if (c == 'u' || c == 'U')
				{
					gtk_text_iter_forward_char(iter);
					return FALSE;
				}
			}
			gtk_text_iter_forward_char(iter);
		}
		return output;
	}
	else if (c == '&')
		return TRUE;
	return FALSE;
}
