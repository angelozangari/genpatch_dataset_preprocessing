PurpleBuddy *
purple_presence_get_buddy(const PurplePresence *presence)
{
	g_return_val_if_fail(presence != NULL, NULL);
	g_return_val_if_fail(purple_presence_get_context(presence) ==
			PURPLE_PRESENCE_CONTEXT_BUDDY, NULL);
	return presence->u.buddy.buddy;
}
