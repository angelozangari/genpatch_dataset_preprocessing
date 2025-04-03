}
void purple_whiteboard_send_draw_list(PurpleWhiteboard *wb, GList *list)
{
	PurpleWhiteboardPrplOps *prpl_ops = wb->prpl_ops;
	if (prpl_ops && prpl_ops->send_draw_list)
		prpl_ops->send_draw_list(wb, list);
}
