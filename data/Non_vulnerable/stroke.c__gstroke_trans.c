gint
_gstroke_trans (gchar *sequence, struct gstroke_metrics *metrics)
{
  GSList *crt_elem;
  /* number of bins recorded in the stroke */
  guint sequence_count = 0;
  /* points-->sequence translation scratch variables */
  gint prev_bin = 0;
  gint current_bin = 0;
  gint bin_count = 0;
  /* flag indicating the start of a stroke - always count it in the sequence */
  gint first_bin = TRUE;
  /* bin boundary and size variables */
  gint delta_x, delta_y;
  gint bound_x_1, bound_x_2;
  gint bound_y_1, bound_y_2;
  /* determine size of grid */
  delta_x = metrics->max_x - metrics->min_x;
  delta_y = metrics->max_y - metrics->min_y;
  /* calculate bin boundary positions */
  bound_x_1 = metrics->min_x + (delta_x / 3);
  bound_x_2 = metrics->min_x + 2 * (delta_x / 3);
  bound_y_1 = metrics->min_y + (delta_y / 3);
  bound_y_2 = metrics->min_y + 2 * (delta_y / 3);
  if (delta_x > GSTROKE_SCALE_RATIO * delta_y) {
    bound_y_1 = (metrics->max_y + metrics->min_y - delta_x) / 2 + (delta_x / 3);
    bound_y_2 = (metrics->max_y + metrics->min_y - delta_x) / 2 + 2 * (delta_x / 3);
  } else if (delta_y > GSTROKE_SCALE_RATIO * delta_x) {
    bound_x_1 = (metrics->max_x + metrics->min_x - delta_y) / 2 + (delta_y / 3);
    bound_x_2 = (metrics->max_x + metrics->min_x - delta_y) / 2 + 2 * (delta_y / 3);
  }
#if 0
  printf ("DEBUG:: point count: %d\n", metrics->point_count);
  printf ("DEBUG:: metrics->min_x: %d\n", metrics->min_x);
  printf ("DEBUG:: metrics->max_x: %d\n", metrics->max_x);
  printf ("DEBUG:: metrics->min_y: %d\n", metrics->min_y);
  printf ("DEBUG:: metrics->max_y: %d\n", metrics->max_y);
  printf ("DEBUG:: delta_x: %d\n", delta_x);
  printf ("DEBUG:: delta_y: %d\n", delta_y);
  printf ("DEBUG:: bound_x_1: %d\n", bound_x_1);
  printf ("DEBUG:: bound_x_2: %d\n", bound_x_2);
  printf ("DEBUG:: bound_y_1: %d\n", bound_y_1);
  printf ("DEBUG:: bound_y_2: %d\n", bound_y_2);
#endif
  /*
    build string by placing points in bins, collapsing bins and
    discarding those with too few points...  */
  crt_elem = metrics->pointList;
  while (crt_elem != NULL)
    {
      /* figure out which bin the point falls in */
      /*printf ("X = %d Y = %d\n", ((p_point)crt_elem->data)->x,
	((p_point)crt_elem->data)->y); */
      current_bin = _gstroke_bin ((p_point)crt_elem->data, bound_x_1,
                                 bound_x_2, bound_y_1, bound_y_2);
      /* if this is the first point, consider it the previous bin, too. */
      if (prev_bin == 0)
	prev_bin = current_bin;
      /*printf ("DEBUG:: current bin: %d x=%d y = %d\n", current_bin,
	      ((p_point)crt_elem->data)->x,
	      ((p_point)crt_elem->data)->y); */
      if (prev_bin == current_bin)
	bin_count++;
      else {
	/* we are moving to a new bin -- consider adding to the sequence */
	if ((bin_count > (metrics->point_count * GSTROKE_BIN_COUNT_PERCENT))
	    || (first_bin == TRUE)) {
		/*
		gchar val = '0' + prev_bin;
		printf ("%c", val);fflush (stdout);
		g_string_append (&sequence, &val);
		*/
	  first_bin = FALSE;
	  sequence[sequence_count++] = '0' + prev_bin;
	  /*  printf ("DEBUG:: adding sequence: %d\n", prev_bin); */
	}
	/* restart counting points in the new bin */
	bin_count=0;
	prev_bin = current_bin;
      }
      /* move to next point, freeing current point from list */
      free (crt_elem->data);
      crt_elem = g_slist_next (crt_elem);
    }
  /* add the last run of points to the sequence */
  sequence[sequence_count++] = '0' + current_bin;
  /*  printf ("DEBUG:: adding final sequence: %d\n", current_bin);  */
  _gstroke_init (metrics);
  {
    /* FIXME: get rid of this block
	  gchar val = '0' + current_bin;
	  printf ("%c\n", val);fflush (stdout);
	  g_string_append (&sequence,  '\0');
	 */
    sequence[sequence_count] = '\0';
  }
  return TRUE;
}
