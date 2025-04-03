}
static void entry_changed_cb(GtkTextBuffer *buffer, void *data)
{
	char *xmlstr, *str;
	GtkTextIter iter;
	int wrapped_lines;
	int lines;
	GdkRectangle oneline;
	int height;
	int pad_top, pad_inside, pad_bottom;
	GtkTextIter start, end;
	xmlnode *node;
	wrapped_lines = 1;
	gtk_text_buffer_get_start_iter(buffer, &iter);
	gtk_text_view_get_iter_location(GTK_TEXT_VIEW(console->entry), &iter, &oneline);
	while (gtk_text_view_forward_display_line(GTK_TEXT_VIEW(console->entry), &iter))
		wrapped_lines++;
	lines = gtk_text_buffer_get_line_count(buffer);
	/* Show a maximum of 64 lines */
	lines = MIN(lines, 6);
	wrapped_lines = MIN(wrapped_lines, 6);
	pad_top = gtk_text_view_get_pixels_above_lines(GTK_TEXT_VIEW(console->entry));
	pad_bottom = gtk_text_view_get_pixels_below_lines(GTK_TEXT_VIEW(console->entry));
	pad_inside = gtk_text_view_get_pixels_inside_wrap(GTK_TEXT_VIEW(console->entry));
	height = (oneline.height + pad_top + pad_bottom) * lines;
	height += (oneline.height + pad_inside) * (wrapped_lines - lines);
	gtk_widget_set_size_request(console->sw, -1, height + 6);
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
       	str = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	if (!str)
		return;
	xmlstr = g_strdup_printf("<xml>%s</xml>", str);
	node = xmlnode_from_str(xmlstr, -1);
	if (node) {
		gtk_imhtml_clear_formatting(GTK_IMHTML(console->entry));
	} else {
		gtk_imhtml_toggle_background(GTK_IMHTML(console->entry), "#ffcece");
	}
	g_free(str);
	g_free(xmlstr);
	if (node)
		xmlnode_free(node);
}
