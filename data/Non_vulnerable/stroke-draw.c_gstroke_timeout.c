static gint
gstroke_timeout (gpointer data)
{
	GtkWidget *widget;
	g_return_val_if_fail(data != NULL, FALSE);
	widget = GTK_WIDGET (data);
	record_stroke_segment (widget);
	return TRUE;
}
