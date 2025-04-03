}
void silcpurple_wb_set_brush(PurpleWhiteboard *wb, int size, int color)
{
	SilcPurpleWb wbs = wb->proto_data;
	wbs->brush_size = size;
	wbs->brush_color = color;
	/* Update whiteboard */
	purple_whiteboard_set_brush(wb, size, color);
}
