static void save_list(void);
static void on_edited(GtkCellRendererText *cellrenderertext,
					  gchar *path, gchar *arg2, gpointer data)
{
	GtkTreeIter iter;
	GValue val;
	if (arg2[0] == '\0') {
		gdk_beep();
		return;
	}
	g_return_if_fail(gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(model), &iter, path));
	val.g_type = 0;
	gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, GPOINTER_TO_INT(data), &val);
	if (strcmp(arg2, g_value_get_string(&val))) {
		gtk_list_store_set(model, &iter, GPOINTER_TO_INT(data), arg2, -1);
		save_list();
	}
	g_value_unset(&val);
}
