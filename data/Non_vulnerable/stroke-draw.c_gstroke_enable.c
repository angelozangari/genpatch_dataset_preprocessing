void
gstroke_enable (GtkWidget *widget)
{
  struct gstroke_metrics*
    metrics = (struct gstroke_metrics *)g_object_get_data(G_OBJECT(widget),
														  GSTROKE_METRICS);
  if (metrics == NULL)
    {
      metrics = (struct gstroke_metrics *)g_malloc (sizeof
                                                    (struct gstroke_metrics));
      metrics->pointList = NULL;
      metrics->min_x = 10000;
      metrics->min_y = 10000;
      metrics->max_x = 0;
      metrics->max_y = 0;
      metrics->point_count = 0;
      g_object_set_data(G_OBJECT(widget), GSTROKE_METRICS, metrics);
      g_signal_connect(G_OBJECT(widget), "event",
					   G_CALLBACK(process_event), NULL);
    }
  else
    _gstroke_init (metrics);
  last_mouse_position.invalid = TRUE;
}
