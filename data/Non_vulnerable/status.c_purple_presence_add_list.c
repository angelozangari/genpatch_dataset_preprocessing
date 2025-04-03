void
purple_presence_add_list(PurplePresence *presence, GList *source_list)
{
	GList *l;
	g_return_if_fail(presence    != NULL);
	g_return_if_fail(source_list != NULL);
	for (l = source_list; l != NULL; l = l->next)
		purple_presence_add_status(presence, (PurpleStatus *)l->data);
}
