static void
silcpurple_wb_parse(SilcPurpleWb wbs, PurpleWhiteboard *wb,
		  unsigned char *message, SilcUInt32 message_len)
{
	SilcUInt8 command;
	SilcUInt16 width, height, brush_size;
	SilcUInt32 brush_color, x, y, dx, dy;
	SilcBufferStruct buf;
	int ret;
	/* Parse the packet */
	silc_buffer_set(&buf, message, message_len);
	ret = silc_buffer_unformat(&buf,
				   SILC_STR_UI_CHAR(&command),
				   SILC_STR_UI_SHORT(&width),
				   SILC_STR_UI_SHORT(&height),
				   SILC_STR_UI_INT(&brush_color),
				   SILC_STR_UI_SHORT(&brush_size),
				   SILC_STR_END);
	if (ret < 0)
		return;
	silc_buffer_pull(&buf, ret);
	/* Update whiteboard if its dimensions changed */
	if (width != wbs->width || height != wbs->height)
		silcpurple_wb_set_dimensions(wb, width, height);
	if (command == SILCPURPLE_WB_DRAW) {
		/* Parse data and draw it */
		ret = silc_buffer_unformat(&buf,
					   SILC_STR_UI_INT(&dx),
					   SILC_STR_UI_INT(&dy),
					   SILC_STR_END);
		if (ret < 0)
			return;
		silc_buffer_pull(&buf, 8);
		x = dx;
		y = dy;
		while (buf.len > 0) {
			ret = silc_buffer_unformat(&buf,
						   SILC_STR_UI_INT(&dx),
						   SILC_STR_UI_INT(&dy),
						   SILC_STR_END);
			if (ret < 0)
				return;
			silc_buffer_pull(&buf, 8);
			purple_whiteboard_draw_line(wb, x, y, x + dx, y + dy,
						  brush_color, brush_size);
			x += dx;
			y += dy;
		}
	}
	if (command == SILCPURPLE_WB_CLEAR)
		purple_whiteboard_clear(wb);
}
