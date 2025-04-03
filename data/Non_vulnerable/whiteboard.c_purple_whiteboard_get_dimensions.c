}
gboolean purple_whiteboard_get_dimensions(const PurpleWhiteboard *wb, int *width, int *height)
{
	PurpleWhiteboardPrplOps *prpl_ops = wb->prpl_ops;
	if (prpl_ops && prpl_ops->get_dimensions)
	{
		prpl_ops->get_dimensions(wb, width, height);
		return TRUE;
	}
	return FALSE;
}
