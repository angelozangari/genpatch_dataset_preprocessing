static gboolean
spellchk_backward_word_start(GtkTextIter *iter)
{
	int output;
	int result;
	output = gtk_text_iter_backward_word_start(iter);
	/* It didn't work...  */
	if (!output)
		return FALSE;
	while (spellchk_inside_word(iter)) {
		result = gtk_text_iter_backward_char(iter);
		/* We can't go backwards anymore?  We're at the beginning of the word. */
		if (!result)
			return TRUE;
		if (!spellchk_inside_word(iter)) {
			gtk_text_iter_forward_char(iter);
			return TRUE;
		}
		output = gtk_text_iter_backward_word_start(iter);
		if (!output)
			return FALSE;
	}
	return TRUE;
}
