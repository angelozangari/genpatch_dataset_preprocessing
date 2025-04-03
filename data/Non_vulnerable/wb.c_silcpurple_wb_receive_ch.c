/* Process incoming whiteboard message on channel */
void silcpurple_wb_receive_ch(SilcClient client, SilcClientConnection conn,
			    SilcClientEntry sender, SilcChannelEntry channel,
			    SilcMessagePayload payload,
			    SilcMessageFlags flags,
			    const unsigned char *message,
			    SilcUInt32 message_len)
{
	SilcPurple sg;
        PurpleConnection *gc;
	PurpleWhiteboard *wb;
	SilcPurpleWb wbs;
	gc = client->application;
        sg = gc->proto_data;
	wb = purple_whiteboard_get_session(sg->account, channel->channel_name);
	if (!wb) {
		/* Ask user if they want to open the whiteboard */
		silcpurple_wb_request(client, message, message_len,
				    sender, channel);
		return;
	}
	wbs = wb->proto_data;
	silcpurple_wb_parse(wbs, wb, (unsigned char *)message, message_len);
}
