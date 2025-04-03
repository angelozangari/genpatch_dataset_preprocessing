/* Called when the server notifies us a message couldn't get sent */
static void message_failed(PurpleConnection *gc, ZNotice_t *notice, struct sockaddr_in from)
{
	if (g_ascii_strcasecmp(notice->z_class, "message")) {
		gchar* chat_failed = g_strdup_printf(
			_("Unable to send to chat %s,%s,%s"),
			notice->z_class, notice->z_class_inst,
			notice->z_recipient);
		purple_notify_error(gc,"",chat_failed,NULL);
		g_free(chat_failed);
	} else {
		purple_notify_error(gc, notice->z_recipient,
			_("User is offline"), NULL);
	}
}
