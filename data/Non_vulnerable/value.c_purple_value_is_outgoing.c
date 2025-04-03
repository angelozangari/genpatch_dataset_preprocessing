gboolean
purple_value_is_outgoing(const PurpleValue *value)
{
	g_return_val_if_fail(value != NULL, FALSE);
	return (value->flags & OUTGOING_FLAG);
}
