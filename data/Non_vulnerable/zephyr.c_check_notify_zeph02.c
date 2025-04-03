}
static gint check_notify_zeph02(gpointer data)
{
	/* XXX add real error reporting */
	PurpleConnection *gc = (PurpleConnection*) data;
	while (ZPending()) {
		ZNotice_t notice;
		struct sockaddr_in from;
		/* XXX add real error reporting */
		z_call_r(ZReceiveNotice(&notice, &from));
		switch (notice.z_kind) {
		case UNSAFE:
		case UNACKED:
		case ACKED:
			handle_message(gc, &notice);
			break;
		case SERVACK:
			if (!(g_ascii_strcasecmp(notice.z_message, ZSRVACK_NOTSENT))) {
				message_failed(gc, &notice, from);
			}
			break;
		case CLIENTACK:
			purple_debug_error("zephyr", "Client ack received\n");
			handle_unknown(&notice); /* XXX: is it really unknown? */
			break;
		default:
			/* we'll just ignore things for now */
			handle_unknown(&notice);
			purple_debug_error("zephyr", "Unhandled notice.\n");
			break;
		}
		/* XXX add real error reporting */
		ZFreeNotice(&notice);
	}
	return TRUE;
}
