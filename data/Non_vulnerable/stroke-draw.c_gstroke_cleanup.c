void
gstroke_cleanup (GtkWidget *widget)
{
  struct gstroke_metrics *metrics;
  GHashTable *hash_table =
    (GHashTable*)g_object_get_data(G_OBJECT(widget), GSTROKE_SIGNALS);
  if (hash_table)
    /*  FIXME: does this delete the elements too?  */
    g_hash_table_destroy (hash_table);
  g_object_steal_data(G_OBJECT(widget), GSTROKE_SIGNALS);
  metrics = (struct gstroke_metrics*)g_object_get_data(G_OBJECT(widget),
													   GSTROKE_METRICS);
  if (metrics)
    g_free (metrics);
  g_object_steal_data(G_OBJECT(widget), GSTROKE_METRICS);
}
