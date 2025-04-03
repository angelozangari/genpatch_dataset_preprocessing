void
purple_presence_set_status_active(PurplePresence *presence, const char *status_id,
		gboolean active)
{
	PurpleStatus *status;
	g_return_if_fail(presence  != NULL);
	g_return_if_fail(status_id != NULL);
	status = purple_presence_get_status(presence, status_id);
	g_return_if_fail(status != NULL);
	/* TODO: Should we do the following? */
	/* g_return_if_fail(active == status->active); */
	if (purple_status_is_exclusive(status))
	{
		if (!active)
		{
			purple_debug_warning("status",
					"Attempted to set a non-independent status "
					"(%s) inactive. Only independent statuses "
					"can be specifically marked inactive.",
					status_id);
			return;
		}
	}
	purple_status_set_active(status, active);
}
