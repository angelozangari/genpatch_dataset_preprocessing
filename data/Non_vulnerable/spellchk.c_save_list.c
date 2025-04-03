}
static void save_list()
{
	GString *data;
	GtkTreeIter iter;
	data = g_string_new("");
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &iter)) {
		do {
			GValue val0;
			GValue val1;
			GValue val2;
			GValue val3;
			val0.g_type = 0;
			val1.g_type = 0;
			val2.g_type = 0;
			val3.g_type = 0;
			gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, BAD_COLUMN, &val0);
			gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, GOOD_COLUMN, &val1);
			gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, WORD_ONLY_COLUMN, &val2);
			gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, CASE_SENSITIVE_COLUMN, &val3);
			g_string_append_printf(data, "COMPLETE %d\nCASE %d\nBAD %s\nGOOD %s\n\n",
								   g_value_get_boolean(&val2),
								   g_value_get_boolean(&val3),
								   g_value_get_string(&val0),
								   g_value_get_string(&val1));
			g_value_unset(&val0);
			g_value_unset(&val1);
			g_value_unset(&val2);
			g_value_unset(&val3);
		} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &iter));
	}
	purple_util_write_data_to_file("dict", data->str, -1);
	g_string_free(data, TRUE);
}
