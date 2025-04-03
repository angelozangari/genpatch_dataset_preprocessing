PurplePresence *
purple_presence_new(PurplePresenceContext context)
{
	PurplePresence *presence;
	g_return_val_if_fail(context != PURPLE_PRESENCE_CONTEXT_UNSET, NULL);
	presence = g_new0(PurplePresence, 1);
	PURPLE_DBUS_REGISTER_POINTER(presence, PurplePresence);
	presence->context = context;
	presence->status_table =
		g_hash_table_new_full(g_str_hash, g_str_equal,
							  g_free, NULL);
	return presence;
}
