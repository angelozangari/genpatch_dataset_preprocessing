}
void silcpurple_wb_get_brush(const PurpleWhiteboard *wb, int *size, int *color)
{
	SilcPurpleWb wbs = wb->proto_data;
	*size = wbs->brush_size;
	*color = wbs->brush_color;
}
