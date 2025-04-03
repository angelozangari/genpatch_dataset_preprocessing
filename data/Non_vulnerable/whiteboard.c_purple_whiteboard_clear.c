}
void purple_whiteboard_clear(PurpleWhiteboard *wb)
{
	if(whiteboard_ui_ops && whiteboard_ui_ops->clear)
		whiteboard_ui_ops->clear(wb);
}
