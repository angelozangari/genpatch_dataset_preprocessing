}
static void remove_sliders() {
	if (window_list) {
		GSList *tmp = window_list;
		while (tmp) {
			slider_win *slidwin = (slider_win*) tmp->data;
			if (slidwin != NULL &&
					GTK_IS_WINDOW(slidwin->win)) {
#if 0
				GtkRequisition slidereq;
				gint width, height;
				/* Figure out how tall the slider was */
				gtk_widget_size_request(
					slidwin->slider, &slidereq);
				gtk_window_get_size(
					GTK_WINDOW(slidwin->win),
					&width, &height);
#endif
				gtk_widget_destroy(slidwin->slider);
#if 0
				gtk_window_resize(
					GTK_WINDOW(slidwin->win),
					width, (height - slidereq.height));
#endif
			}
			g_free(slidwin);
			tmp = tmp->next;
		}
		g_slist_free(window_list);
		window_list = NULL;
	}
}
}
static void remove_sliders() {
	if (window_list) {
		GSList *tmp = window_list;
		while (tmp) {
			slider_win *slidwin = (slider_win*) tmp->data;
			if (slidwin != NULL &&
					GTK_IS_WINDOW(slidwin->win)) {
#if 0
				GtkRequisition slidereq;
				gint width, height;
				/* Figure out how tall the slider was */
				gtk_widget_size_request(
					slidwin->slider, &slidereq);
				gtk_window_get_size(
					GTK_WINDOW(slidwin->win),
					&width, &height);
#endif
				gtk_widget_destroy(slidwin->slider);
#if 0
				gtk_window_resize(
					GTK_WINDOW(slidwin->win),
					width, (height - slidereq.height));
#endif
			}
			g_free(slidwin);
			tmp = tmp->next;
		}
		g_slist_free(window_list);
		window_list = NULL;
	}
}
