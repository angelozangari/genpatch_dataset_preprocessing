PurpleValue *
purple_status_attr_get_value(const PurpleStatusAttr *attr)
{
	g_return_val_if_fail(attr != NULL, NULL);
	return attr->value_type;
}
