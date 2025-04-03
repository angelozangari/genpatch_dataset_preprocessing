}
static void word_only_toggled(GtkCellRendererToggle *cellrenderertoggle,
						gchar *path, gpointer data){
	GtkTreeIter iter;
	gboolean enabled;
	g_return_if_fail(gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(model), &iter, path));
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
					   WORD_ONLY_COLUMN, &enabled,
					   -1);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
					   WORD_ONLY_COLUMN, !enabled,
					   -1);
	/* I want to be sure that the above change has happened to the GtkTreeView first. */
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
					   CASE_SENSITIVE_COLUMN, enabled,
					   -1);
	save_list();
}
