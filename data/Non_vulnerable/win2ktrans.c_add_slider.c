}
static void add_slider(GtkWidget *win) {
	GList *wl, *wl1;
	GtkWidget *vbox = NULL;
	/* Look up this window to see if it already has a slider */
	if (!find_slidwin(win)) {
		GtkWidget *slider_box = NULL;
		slider_win *slidwin = NULL;
		GtkRequisition slidereq;
		gint width, height;
		/* Get top vbox */
		for (wl1 = wl = gtk_container_get_children(
					GTK_CONTAINER(win));
				wl != NULL;
				wl = wl->next) {
			if (GTK_IS_VBOX(GTK_OBJECT(wl->data)))
				vbox = GTK_WIDGET(wl->data);
			else {
				purple_debug_error(WINTRANS_PLUGIN_ID,
					"no vbox found\n");
				return;
			}
		}
		g_list_free(wl1);
		slider_box = wintrans_slider(win);
		/* Figure out how tall the slider wants to be */
		gtk_widget_size_request(slider_box, &slidereq);
		gtk_window_get_size(GTK_WINDOW(win), &width, &height);
		gtk_box_pack_start(GTK_BOX(vbox),
			slider_box, FALSE, FALSE, 0);
#if 0 /*Now that we save window sizes, don't resize it or else it causes windows to grow*/
		/* Make window taller so we don't slowly collapse its message area */
		gtk_window_resize(GTK_WINDOW(win), width,
			(height + slidereq.height));
#endif
		/* Add window to list, to track that it has a slider */
		slidwin = g_new0(slider_win, 1);
		slidwin->win = win;
		slidwin->slider = slider_box;
		window_list = g_slist_append(window_list, slidwin);
	}
}
