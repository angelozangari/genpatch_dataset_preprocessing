static gchar *
substitute_word(gchar *word)
{
	GtkTreeIter iter;
	gchar *outword;
	gchar *lowerword;
	gchar *foldedword;
	if (word == NULL)
		return NULL;
	lowerword = g_utf8_strdown(word, -1);
	foldedword = g_utf8_casefold(word, -1);
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &iter)) {
		do {
			GValue val1;
			gboolean case_sensitive;
			const char *bad;
			gchar *tmpbad = NULL;
			val1.g_type = 0;
			gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, WORD_ONLY_COLUMN, &val1);
			if (!g_value_get_boolean(&val1)) {
				g_value_unset(&val1);
				continue;
			}
			g_value_unset(&val1);
			gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, CASE_SENSITIVE_COLUMN, &val1);
			case_sensitive = g_value_get_boolean(&val1);
			g_value_unset(&val1);
			gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, BAD_COLUMN, &val1);
			bad = g_value_get_string(&val1);
			if ((case_sensitive && !strcmp(bad, word)) ||
			    (!case_sensitive && (!strcmp(bad, lowerword) ||
			                        (!is_word_lowercase(bad) &&
			                         !strcmp((tmpbad = g_utf8_casefold(bad, -1)), foldedword)))))
			{
				GValue val2;
				const char *good;
				g_free(tmpbad);
				val2.g_type = 0;
				gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, GOOD_COLUMN, &val2);
				good = g_value_get_string(&val2);
				if (!case_sensitive && is_word_lowercase(bad) && is_word_lowercase(good))
				{
					if (is_word_uppercase(word))
						outword = g_utf8_strup(good, -1);
					else if (is_word_proper(word))
						outword = make_word_proper(good);
					else
						outword = g_strdup(good);
				}
				else
					outword = g_strdup(good);
				g_value_unset(&val1);
				g_value_unset(&val2);
				g_free(lowerword);
				g_free(foldedword);
				return outword;
			}
			g_value_unset(&val1);
			g_free(tmpbad);
		} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &iter));
	}
	g_free(lowerword);
	g_free(foldedword);
	return NULL;
}
