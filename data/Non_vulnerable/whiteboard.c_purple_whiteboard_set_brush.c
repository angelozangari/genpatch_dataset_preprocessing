}
void purple_whiteboard_set_brush(PurpleWhiteboard *wb, int size, int color)
{
	if (whiteboard_ui_ops && whiteboard_ui_ops->set_brush)
		whiteboard_ui_ops->set_brush(wb, size, color);
}
