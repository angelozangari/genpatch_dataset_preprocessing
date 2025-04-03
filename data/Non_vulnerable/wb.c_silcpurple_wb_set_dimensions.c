}
void silcpurple_wb_set_dimensions(PurpleWhiteboard *wb, int width, int height)
{
	SilcPurpleWb wbs = wb->proto_data;
	wbs->width = width > SILCPURPLE_WB_WIDTH_MAX ? SILCPURPLE_WB_WIDTH_MAX :
			width;
	wbs->height = height > SILCPURPLE_WB_HEIGHT_MAX ? SILCPURPLE_WB_HEIGHT_MAX :
			height;
	/* Update whiteboard */
	purple_whiteboard_set_dimensions(wb, wbs->width, wbs->height);
}
