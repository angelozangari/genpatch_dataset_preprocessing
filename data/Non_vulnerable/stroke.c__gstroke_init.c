void
_gstroke_init (struct gstroke_metrics *metrics)
{
  if (metrics->pointList != NULL) {
    /* FIXME: does this free the data too?*/
    g_slist_free (metrics->pointList);
    metrics->pointList = NULL;
    metrics->point_count = 0;
  }
}
