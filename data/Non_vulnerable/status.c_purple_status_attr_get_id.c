const char *
purple_status_attr_get_id(const PurpleStatusAttr *attr)
{
	g_return_val_if_fail(attr != NULL, NULL);
	return attr->id;
}
