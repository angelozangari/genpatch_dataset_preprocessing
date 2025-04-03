void
purple_value_destroy(PurpleValue *value)
{
	g_return_if_fail(value != NULL);
	if (purple_value_get_type(value) == PURPLE_TYPE_BOXED)
	{
		g_free(value->u.specific_type);
	}
	else if (purple_value_get_type(value) == PURPLE_TYPE_STRING)
	{
		g_free(value->data.string_data);
	}
	g_free(value);
}
