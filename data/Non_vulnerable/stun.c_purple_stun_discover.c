}
PurpleStunNatDiscovery *purple_stun_discover(StunCallback cb) {
	const char *servername = purple_prefs_get_string("/purple/network/stun_server");
	purple_debug_info("stun", "using server %s\n", servername);
	if(nattype.status == PURPLE_STUN_STATUS_DISCOVERING) {
		if(cb)
			callbacks = g_slist_append(callbacks, cb);
		return &nattype;
	}
	if(nattype.status != PURPLE_STUN_STATUS_UNDISCOVERED) {
		gboolean use_cached_result = TRUE;
		/** Deal with the server name having changed since we did the
		    lookup */
		if (servername && strlen(servername) > 1
				&& !purple_strequal(servername, nattype.servername)) {
			use_cached_result = FALSE;
		}
		/* If we don't have a successful status and it has been 5
		   minutes since we last did a lookup, redo the lookup */
		if (nattype.status != PURPLE_STUN_STATUS_DISCOVERED
				&& (time(NULL) - nattype.lookup_time) > 300) {
			use_cached_result = FALSE;
		}
		if (use_cached_result) {
			if(cb)
				purple_timeout_add(10, call_callback, cb);
			return &nattype;
		}
	}
	if(!servername || (strlen(servername) < 2)) {
		nattype.status = PURPLE_STUN_STATUS_UNKNOWN;
		nattype.lookup_time = time(NULL);
		if(cb)
			purple_timeout_add(10, call_callback, cb);
		return &nattype;
	}
	nattype.status = PURPLE_STUN_STATUS_DISCOVERING;
	nattype.publicip[0] = '\0';
	g_free(nattype.servername);
	nattype.servername = g_strdup(servername);
	callbacks = g_slist_append(callbacks, cb);
	purple_srv_resolve_account(NULL, "stun", "udp", servername, do_test1,
		(gpointer) servername);
	return &nattype;
}
