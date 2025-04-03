PurpleStatusAttr *
purple_status_type_get_attr(const PurpleStatusType *status_type, const char *id)
{
	GList *l;
	g_return_val_if_fail(status_type != NULL, NULL);
	g_return_val_if_fail(id          != NULL, NULL);
	for (l = status_type->attrs; l != NULL; l = l->next)
	{
		PurpleStatusAttr *attr = (PurpleStatusAttr *)l->data;
		if (purple_strequal(purple_status_attr_get_id(attr), id))
			return attr;
	}
	return NULL;
}
