static void
record_stroke_segment (GtkWidget *widget)
{
  gint x, y;
  struct gstroke_metrics *metrics;
  g_return_if_fail( widget != NULL );
  gtk_widget_get_pointer (widget, &x, &y);
  if (last_mouse_position.invalid)
    last_mouse_position.invalid = FALSE;
  else if (gstroke_draw_strokes())
    {
#if 1
      XDrawLine (gstroke_disp, gstroke_window, gstroke_gc,
                 last_mouse_position.last_point.x,
                 last_mouse_position.last_point.y,
                 x, y);
      /* XFlush (gstroke_disp); */
#else
      /* FIXME: this does not work. It will only work if we create a
         corresponding GDK window for stroke_window and draw on
         that... */
      gdk_draw_line (gtk_widget_get_window(widget),
                     widget->style->fg_gc[GTK_STATE_NORMAL],
                     last_mouse_position.last_point.x,
                     last_mouse_position.last_point.y,
                     x,
                     y);
#endif
    }
  if (last_mouse_position.last_point.x != x
      || last_mouse_position.last_point.y != y)
    {
      last_mouse_position.last_point.x = x;
      last_mouse_position.last_point.y = y;
      metrics = (struct gstroke_metrics *)g_object_get_data(G_OBJECT(widget),
															GSTROKE_METRICS);
      _gstroke_record (x, y, metrics);
    }
}
