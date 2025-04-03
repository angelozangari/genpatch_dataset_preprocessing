}
static void gstroke_cancel(GdkEvent *event)
{
	last_mouse_position.invalid = TRUE;
	if (timer_id > 0)
	    g_source_remove (timer_id);
	timer_id = 0;
	if( event != NULL )
		gdk_pointer_ungrab (event->button.time);
	if (gstroke_draw_strokes() && gstroke_disp != NULL) {
	    /* get rid of the invisible stroke window */
	    XUnmapWindow (gstroke_disp, gstroke_window);
	    XFlush (gstroke_disp);
	}
}
