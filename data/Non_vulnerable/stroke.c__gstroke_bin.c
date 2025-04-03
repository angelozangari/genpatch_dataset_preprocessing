static gint
_gstroke_bin (p_point point_p, gint bound_x_1, gint bound_x_2,
	    gint bound_y_1, gint bound_y_2)
{
  gint bin_num = 1;
  if (point_p->x > bound_x_1) bin_num += 1;
  if (point_p->x > bound_x_2) bin_num += 1;
  if (point_p->y > bound_y_1) bin_num += 3;
  if (point_p->y > bound_y_2) bin_num += 3;
  return bin_num;
}
