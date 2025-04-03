}
void purple_whiteboard_send_brush(PurpleWhiteboard *wb, int size, int color)
{
	PurpleWhiteboardPrplOps *prpl_ops = wb->prpl_ops;
	if (prpl_ops && prpl_ops->set_brush)
		prpl_ops->set_brush(wb, size, color);
}
