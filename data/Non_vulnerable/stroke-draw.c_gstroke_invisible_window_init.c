static void
gstroke_invisible_window_init (GtkWidget *widget)
{
  XSetWindowAttributes w_attr;
  XWindowAttributes orig_w_attr;
  unsigned long mask, col_border, col_background;
  unsigned int border_width;
  XSizeHints hints;
  Display *disp = GDK_WINDOW_XDISPLAY(gtk_widget_get_window(widget));
  Window wind = GDK_WINDOW_XWINDOW (gtk_widget_get_window(widget));
  int screen = DefaultScreen (disp);
	if (!gstroke_draw_strokes())
		return;
  gstroke_disp = disp;
  /* X server should save what's underneath */
  XGetWindowAttributes (gstroke_disp, wind, &orig_w_attr);
  hints.x = orig_w_attr.x;
  hints.y = orig_w_attr.y;
  hints.width = orig_w_attr.width;
  hints.height = orig_w_attr.height;
  mask = CWSaveUnder;
  w_attr.save_under = True;
  /* inhibit all the decorations */
  mask |= CWOverrideRedirect;
  w_attr.override_redirect = True;
  /* Don't set a background, transparent window */
  mask |= CWBackPixmap;
  w_attr.background_pixmap = None;
  /* Default input window look */
  col_background = WhitePixel (gstroke_disp, screen);
  /* no border for the window */
#if 0
  border_width = 5;
#endif
  border_width = 0;
  col_border = BlackPixel (gstroke_disp, screen);
  gstroke_window = XCreateSimpleWindow (gstroke_disp, wind,
                                        0, 0,
                                        hints.width - 2 * border_width,
                                        hints.height - 2 * border_width,
                                        border_width,
                                        col_border, col_background);
  gstroke_gc = XCreateGC (gstroke_disp, gstroke_window, 0, NULL);
  XSetFunction (gstroke_disp, gstroke_gc, GXinvert);
  XChangeWindowAttributes (gstroke_disp, gstroke_window, mask, &w_attr);
  XSetLineAttributes (gstroke_disp, gstroke_gc, 2, LineSolid,
                      CapButt, JoinMiter);
  XMapRaised (gstroke_disp, gstroke_window);
#if 0
  /*FIXME: is this call really needed? If yes, does it need the real
    argc and argv? */
  hints.flags = PPosition | PSize;
  XSetStandardProperties (gstroke_disp, gstroke_window, "gstroke_test", NULL,
                          (Pixmap)NULL, NULL, 0, &hints);
  /* Receive the close window client message */
  {
    /* FIXME: is this really needed? If yes, something should be done
       with wmdelete...*/
    Atom wmdelete = XInternAtom (gstroke_disp, "WM_DELETE_WINDOW",
                                 False);
    XSetWMProtocols (gstroke_disp, gstroke_window, &wmdelete, True);
  }
#endif
}
