}
static unsigned int zephyr_send_typing(PurpleConnection *gc, const char *who, PurpleTypingState state) {
	gchar *recipient;
	zephyr_account *zephyr = gc->proto_data;
	if (use_tzc(zephyr))
		return 0;
	if (state == PURPLE_NOT_TYPING)
		return 0;
	/* XXX We probably should care if this fails. Or maybe we don't want to */
	if (!who) {
		purple_debug_info("zephyr", "who is null\n");
		recipient = local_zephyr_normalize(zephyr,"");
	} else {
		char *comma = strrchr(who, ',');
		/* Don't ping broadcast (chat) recipients */
		/* The strrchr case finds a realm-stripped broadcast subscription
		   e.g. comma is the last character in the string */
		if (comma && ( (*(comma+1) == '\0') || (*(comma+1) == '@')))
			return 0;
		recipient = local_zephyr_normalize(zephyr,who);
	}
	purple_debug_info("zephyr","about to send typing notification to %s\n",recipient);
	zephyr_send_message(zephyr,"MESSAGE","PERSONAL",recipient,"","","PING");
	purple_debug_info("zephyr","sent typing notification\n");
	/*
	 * TODO: Is this correct?  It means we will call
	 *       serv_send_typing(gc, who, PURPLE_TYPING) once every 15 seconds
	 *       until the Purple user stops typing.
	 */
	return ZEPHYR_TYPING_SEND_TIMEOUT;
}
