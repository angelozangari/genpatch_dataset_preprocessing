}
gboolean purple_whiteboard_get_brush(const PurpleWhiteboard *wb, int *size, int *color)
{
	PurpleWhiteboardPrplOps *prpl_ops = wb->prpl_ops;
	if (prpl_ops && prpl_ops->get_brush)
	{
		prpl_ops->get_brush(wb, size, color);
		return TRUE;
	}
	return FALSE;
}
