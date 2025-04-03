}
void purple_whiteboard_start(PurpleWhiteboard *wb)
{
	/* Create frontend for whiteboard */
	if(whiteboard_ui_ops && whiteboard_ui_ops->create)
		whiteboard_ui_ops->create(wb);
}
