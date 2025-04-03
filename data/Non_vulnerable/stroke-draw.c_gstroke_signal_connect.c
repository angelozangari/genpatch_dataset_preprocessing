guint
gstroke_signal_connect (GtkWidget *widget,
                        const gchar *name,
                        void (*func)(GtkWidget *widget, void *data),
                        gpointer data)
{
  struct gstroke_func_and_data *func_and_data;
  GHashTable *hash_table =
    (GHashTable*)g_object_get_data(G_OBJECT(widget), GSTROKE_SIGNALS);
  if (!hash_table)
    {
      hash_table = g_hash_table_new (g_str_hash, g_str_equal);
      g_object_set_data(G_OBJECT(widget), GSTROKE_SIGNALS,
						(gpointer)hash_table);
    }
  func_and_data = g_new (struct gstroke_func_and_data, 1);
  func_and_data->func = func;
  func_and_data->data = data;
  g_hash_table_insert (hash_table, (gpointer)name, (gpointer)func_and_data);
  return TRUE;
}
