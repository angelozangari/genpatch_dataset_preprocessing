PurplePresenceContext
purple_presence_get_context(const PurplePresence *presence)
{
	g_return_val_if_fail(presence != NULL, PURPLE_PRESENCE_CONTEXT_UNSET);
	return presence->context;
}
