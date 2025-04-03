void
_gstroke_record (gint x, gint y, struct gstroke_metrics *metrics)
{
  p_point new_point_p;
  gint delx, dely;
  float ix, iy;
  g_return_if_fail( metrics != NULL );
#if 0
  printf ("%d:%d ", x, y); fflush (stdout);
#endif
  if (metrics->point_count < GSTROKE_MAX_POINTS) {
    new_point_p = (p_point) g_malloc (sizeof (struct s_point));
    if (metrics->pointList == NULL) {
      /* first point in list - initialize metrics */
      metrics->min_x = 10000;
      metrics->min_y = 10000;
      metrics->max_x = -1;
      metrics->max_y = -1;
      metrics->pointList = g_slist_prepend(metrics->pointList, new_point_p);
      metrics->point_count = 0;
    } else {
#define LAST_POINT ((p_point)(g_slist_last (metrics->pointList)->data))
      /* interpolate between last and current point */
      delx = x - LAST_POINT->x;
      dely = y - LAST_POINT->y;
      if (abs(delx) > abs(dely)) {  /* step by the greatest delta direction */
	iy = LAST_POINT->y;
	/* go from the last point to the current, whatever direction it may be */
	for (ix = LAST_POINT->x; (delx > 0) ? (ix < x) : (ix > x); ix += (delx > 0) ? 1 : -1) {
	  /* step the other axis by the correct increment */
	  iy += fabs(((float) dely / (float) delx)) * (float) ((dely < 0) ? -1.0 : 1.0);
	  /* add the interpolated point */
	  new_point_p->x = ix;
	  new_point_p->y = iy;
	  metrics->pointList = g_slist_append (metrics->pointList, new_point_p);
	  /* update metrics */
	  if (((gint) ix) < metrics->min_x) metrics->min_x = (gint) ix;
	  if (((gint) ix) > metrics->max_x) metrics->max_x = (gint) ix;
	  if (((gint) iy) < metrics->min_y) metrics->min_y = (gint) iy;
	  if (((gint) iy) > metrics->max_y) metrics->max_y = (gint) iy;
	  metrics->point_count++;
	  new_point_p = (p_point) malloc (sizeof(struct s_point));
	}
      } else {  /* same thing, but for dely larger than delx case... */
	ix = LAST_POINT->x;
	/* go from the last point to the current, whatever direction it may be
	 */
	for (iy = LAST_POINT->y; (dely > 0) ? (iy < y) : (iy > y); iy += (dely > 0) ? 1 : -1) {
	  /* step the other axis by the correct increment */
	  ix += fabs(((float) delx / (float) dely)) * (float) ((delx < 0) ? -1.0 : 1.0);
	  /* add the interpolated point */
	  new_point_p->y = iy;
	  new_point_p->x = ix;
	  metrics->pointList = g_slist_append(metrics->pointList, new_point_p);
	  /* update metrics */
	  if (((gint) ix) < metrics->min_x) metrics->min_x = (gint) ix;
	  if (((gint) ix) > metrics->max_x) metrics->max_x = (gint) ix;
	  if (((gint) iy) < metrics->min_y) metrics->min_y = (gint) iy;
	  if (((gint) iy) > metrics->max_y) metrics->max_y = (gint) iy;
	  metrics->point_count++;
	  new_point_p = (p_point) malloc (sizeof(struct s_point));
	}
      }
      /* add the sampled point */
      metrics->pointList = g_slist_append(metrics->pointList, new_point_p);
    }
    /* record the sampled point values */
    new_point_p->x = x;
    new_point_p->y = y;
#if 0
    {
      GSList *crt = metrics->pointList;
      printf ("Record ");
      while (crt != NULL)
	{
	  printf ("(%d,%d)", ((p_point)crt->data)->x, ((p_point)crt->data)->y);
	  crt = g_slist_next (crt);
	}
      printf ("\n");
    }
#endif
  }
}
