PurpleStatusType *
purple_status_type_new(PurpleStatusPrimitive primitive, const char *id,
					 const char *name, gboolean user_settable)
{
	g_return_val_if_fail(primitive != PURPLE_STATUS_UNSET, NULL);
	return purple_status_type_new_full(primitive, id, name, TRUE,
			user_settable, FALSE);
}
