static gboolean
substitute_simple_buffer(GtkTextBuffer *buffer)
{
	GtkTextIter start;
	GtkTextIter end;
	GtkTreeIter treeiter;
	gchar *text = NULL;
	gtk_text_buffer_get_iter_at_offset(buffer, &start, 0);
	gtk_text_buffer_get_iter_at_offset(buffer, &end, 0);
	gtk_text_iter_forward_to_end(&end);
	text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &treeiter) && text) {
		do {
			GValue val1;
			const gchar *bad;
			gchar *cursor;
			glong char_pos;
			val1.g_type = 0;
			gtk_tree_model_get_value(GTK_TREE_MODEL(model), &treeiter, WORD_ONLY_COLUMN, &val1);
			if (g_value_get_boolean(&val1))
			{
				g_value_unset(&val1);
				continue;
			}
			g_value_unset(&val1);
			gtk_tree_model_get_value(GTK_TREE_MODEL(model), &treeiter, BAD_COLUMN, &val1);
			bad = g_value_get_string(&val1);
			/* using g_utf8_* to get /character/ offsets instead of byte offsets for buffer */
			if ((cursor = g_strrstr(text, bad)))
			{
				GValue val2;
				const gchar *good;
				val2.g_type = 0;
				gtk_tree_model_get_value(GTK_TREE_MODEL(model), &treeiter, GOOD_COLUMN, &val2);
				good = g_value_get_string(&val2);
				char_pos = g_utf8_pointer_to_offset(text, cursor);
				gtk_text_buffer_get_iter_at_offset(buffer, &start, char_pos);
				gtk_text_buffer_get_iter_at_offset(buffer, &end, char_pos + g_utf8_strlen(bad, -1));
				gtk_text_buffer_delete(buffer, &start, &end);
				gtk_text_buffer_get_iter_at_offset(buffer, &start, char_pos);
				gtk_text_buffer_insert(buffer, &start, good, -1);
				g_value_unset(&val2);
				g_free(text);
				g_value_unset(&val1);
				return TRUE;
			}
			g_value_unset(&val1);
		} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &treeiter));
	}
	g_free(text);
	return FALSE;
}
