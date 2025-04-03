}
void purple_whiteboard_destroy(PurpleWhiteboard *wb)
{
	g_return_if_fail(wb != NULL);
	if(wb->ui_data)
	{
		/* Destroy frontend */
		if(whiteboard_ui_ops && whiteboard_ui_ops->destroy)
			whiteboard_ui_ops->destroy(wb);
	}
	/* Do protocol specific session ending procedures */
	if(wb->prpl_ops && wb->prpl_ops->end)
		wb->prpl_ops->end(wb);
	g_free(wb->who);
	wbList = g_list_remove(wbList, wb);
	g_free(wb);
}
