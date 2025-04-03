}
static slider_win* find_slidwin(GtkWidget *win) {
	GSList *tmp = window_list;
	while (tmp) {
		if (((slider_win*) (tmp->data))->win == win)
			return (slider_win*) tmp->data;
		tmp = tmp->next;
	}
	return NULL;
}
