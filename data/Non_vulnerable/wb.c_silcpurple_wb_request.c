static void
silcpurple_wb_request(SilcClient client, const unsigned char *message,
		    SilcUInt32 message_len, SilcClientEntry sender,
		    SilcChannelEntry channel)
{
	char tmp[128];
	SilcPurpleWbRequest req;
	PurpleConnection *gc;
	SilcPurple sg;
	gc = client->application;
	sg = gc->proto_data;
	/* Open whiteboard automatically if requested */
	if (purple_account_get_bool(sg->account, "open-wb", FALSE)) {
		PurpleWhiteboard *wb;
		if (!channel)
			wb = silcpurple_wb_init(sg, sender);
		else
			wb = silcpurple_wb_init_ch(sg, channel);
		silcpurple_wb_parse(wb->proto_data, wb, (unsigned char *)message,
				  message_len);
		return;
	}
	/* Close any previous unaccepted requests */
	purple_request_close_with_handle(sender);
	if (!channel) {
		g_snprintf(tmp, sizeof(tmp),
			_("%s sent message to whiteboard. Would you like "
			  "to open the whiteboard?"), sender->nickname);
	} else {
		g_snprintf(tmp, sizeof(tmp),
			_("%s sent message to whiteboard on %s channel. "
			  "Would you like to open the whiteboard?"),
			sender->nickname, channel->channel_name);
	}
	req = silc_calloc(1, sizeof(*req));
	if (!req)
		return;
	req->message = silc_memdup(message, message_len);
	req->message_len = message_len;
	req->sender = sender;
	req->channel = channel;
	req->sg = sg;
	purple_request_action(sender, _("Whiteboard"), tmp, NULL, 1,
				sg->account, sender->nickname, NULL, req, 2,
			    _("Yes"), G_CALLBACK(silcpurple_wb_request_cb),
			    _("No"), G_CALLBACK(silcpurple_wb_request_cb));
}
