static gint
process_event (GtkWidget *widget, GdkEvent *event, gpointer data G_GNUC_UNUSED)
{
  static GtkWidget *original_widget = NULL;
  static GdkCursor *cursor = NULL;
  switch (event->type) {
    case GDK_BUTTON_PRESS:
		if (event->button.button != gstroke_get_mouse_button()) {
			/* Similar to the bug below catch when any other button is
			 * clicked after the middle button is clicked (but possibly
			 * not released)
			 */
			gstroke_cancel(event);
			original_widget = NULL;
			break;
		}
      original_widget = widget; /* remeber the widget where
                                   the stroke started */
      gstroke_invisible_window_init (widget);
      record_stroke_segment (widget);
	  if (cursor == NULL)
		  cursor = gdk_cursor_new(GDK_PENCIL);
      gdk_pointer_grab (gtk_widget_get_window(widget), FALSE,
			GDK_BUTTON_RELEASE_MASK, NULL, cursor,
			event->button.time);
      timer_id = g_timeout_add (GSTROKE_TIMEOUT_DURATION,
				  gstroke_timeout, widget);
      return TRUE;
    case GDK_BUTTON_RELEASE:
      if ((event->button.button != gstroke_get_mouse_button())
	  || (original_widget == NULL)) {
		/* Nice bug when you hold down one button and press another. */
		/* We'll just cancel the gesture instead. */
		gstroke_cancel(event);
		original_widget = NULL;
		break;
	  }
      last_mouse_position.invalid = TRUE;
      original_widget = NULL;
      g_source_remove (timer_id);
      gdk_pointer_ungrab (event->button.time);
      timer_id = 0;
      {
	char result[GSTROKE_MAX_SEQUENCE];
	struct gstroke_metrics *metrics;
	metrics = (struct gstroke_metrics *)g_object_get_data(G_OBJECT (widget),
														  GSTROKE_METRICS);
		if (gstroke_draw_strokes()) {
			/* get rid of the invisible stroke window */
			XUnmapWindow (gstroke_disp, gstroke_window);
			XFlush (gstroke_disp);
		}
	_gstroke_canonical (result, metrics);
	gstroke_execute (widget, result);
      }
      return FALSE;
    default:
      break;
  }
  return FALSE;
}
