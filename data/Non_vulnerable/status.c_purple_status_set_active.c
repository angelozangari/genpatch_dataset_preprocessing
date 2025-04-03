void
purple_status_set_active(PurpleStatus *status, gboolean active)
{
	purple_status_set_active_with_attrs_list(status, active, NULL);
}
