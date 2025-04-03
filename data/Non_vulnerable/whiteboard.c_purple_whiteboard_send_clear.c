}
void purple_whiteboard_send_clear(PurpleWhiteboard *wb)
{
	PurpleWhiteboardPrplOps *prpl_ops = wb->prpl_ops;
	if (prpl_ops && prpl_ops->clear)
		prpl_ops->clear(wb);
}
