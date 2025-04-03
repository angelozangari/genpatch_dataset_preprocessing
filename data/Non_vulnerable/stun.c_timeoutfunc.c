}
static gboolean timeoutfunc(gpointer data) {
	struct stun_conn *sc = data;
	if(sc->retry >= 2) {
		purple_debug_warning("stun", "request timed out, giving up.\n");
		if(sc->test == 2)
			nattype.type = PURPLE_STUN_NAT_TYPE_SYMMETRIC;
		/* set unknown */
		nattype.status = PURPLE_STUN_STATUS_UNKNOWN;
		nattype.lookup_time = time(NULL);
		/* callbacks */
		do_callbacks();
		/* we don't need to remove the timeout (returning FALSE) */
		sc->timeout = 0;
		close_stun_conn(sc);
		return FALSE;
	}
	purple_debug_info("stun", "request timed out, retrying.\n");
	sc->retry++;
	if (sendto(sc->fd, sc->packet, sc->packetsize, 0,
		(struct sockaddr *)&(sc->addr), sizeof(struct sockaddr_in)) !=
		(gssize)sc->packetsize)
	{
		purple_debug_warning("stun", "sendto failed\n");
		return FALSE;
	}
	return TRUE;
}
