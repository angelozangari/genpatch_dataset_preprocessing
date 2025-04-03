}
PurpleWhiteboard *silcpurple_wb_init_ch(SilcPurple sg, SilcChannelEntry channel)
{
	PurpleWhiteboard *wb;
	SilcPurpleWb wbs;
	wb = purple_whiteboard_get_session(sg->account, channel->channel_name);
	if (!wb)
		wb = purple_whiteboard_create(sg->account, channel->channel_name, 0);
	if (!wb)
		return NULL;
	if (!wb->proto_data) {
		wbs = silc_calloc(1, sizeof(*wbs));
		if (!wbs)
			return NULL;
		wbs->type = 1;
		wbs->u.channel = channel;
		wbs->width = SILCPURPLE_WB_WIDTH;
		wbs->height = SILCPURPLE_WB_HEIGHT;
		wbs->brush_size = SILCPURPLE_WB_BRUSH_SMALL;
		wbs->brush_color = SILCPURPLE_WB_COLOR_BLACK;
		wb->proto_data = wbs;
		/* Start the whiteboard */
		purple_whiteboard_start(wb);
		purple_whiteboard_clear(wb);
	}
	return wb;
}
