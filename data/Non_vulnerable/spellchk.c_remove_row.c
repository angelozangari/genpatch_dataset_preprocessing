}
static void remove_row(void *data1, gpointer data2)
{
	GtkTreeRowReference *row_reference;
	GtkTreePath *path;
	GtkTreeIter iter;
	row_reference = (GtkTreeRowReference *)data1;
	path = gtk_tree_row_reference_get_path(row_reference);
	if (gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path))
		gtk_list_store_remove(model, &iter);
	gtk_tree_path_free(path);
	gtk_tree_row_reference_free(row_reference);
}
