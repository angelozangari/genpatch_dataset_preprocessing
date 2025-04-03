void
purple_status_set_active_with_attrs(PurpleStatus *status, gboolean active, va_list args)
{
	GList *attrs = NULL;
	const gchar *id;
	gpointer data;
	while ((id = va_arg(args, const char *)) != NULL)
	{
		attrs = g_list_append(attrs, (char *)id);
		data = va_arg(args, void *);
		attrs = g_list_append(attrs, data);
	}
	purple_status_set_active_with_attrs_list(status, active, attrs);
	g_list_free(attrs);
}
