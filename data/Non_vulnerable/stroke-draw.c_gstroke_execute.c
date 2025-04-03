static void
gstroke_execute (GtkWidget *widget, const gchar *name)
{
  GHashTable *hash_table =
    (GHashTable*)g_object_get_data(G_OBJECT(widget), GSTROKE_SIGNALS);
#if 0
  purple_debug(PURPLE_DEBUG_MISC, "gestures", "gstroke %s\n", name);
#endif
  if (hash_table)
    {
      struct gstroke_func_and_data *fd =
	(struct gstroke_func_and_data*)g_hash_table_lookup (hash_table, name);
      if (fd)
	(*fd->func)(widget, fd->data);
    }
}
