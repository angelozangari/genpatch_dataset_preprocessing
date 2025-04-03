}
void silcpurple_wb_get_dimensions(const PurpleWhiteboard *wb, int *width, int *height)
{
	SilcPurpleWb wbs = wb->proto_data;
	*width = wbs->width;
	*height = wbs->height;
}
