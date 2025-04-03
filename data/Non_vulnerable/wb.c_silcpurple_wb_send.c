/* Send whiteboard message */
void silcpurple_wb_send(PurpleWhiteboard *wb, GList *draw_list)
{
	SilcPurpleWb wbs = wb->proto_data;
	SilcBuffer packet;
	GList *list;
	int len;
        PurpleConnection *gc;
        SilcPurple sg;
	g_return_if_fail(draw_list);
	gc = purple_account_get_connection(wb->account);
	g_return_if_fail(gc);
 	sg = gc->proto_data;
	g_return_if_fail(sg);
	len = SILCPURPLE_WB_HEADER;
	for (list = draw_list; list; list = list->next)
		len += 4;
	packet = silc_buffer_alloc_size(len);
	if (!packet)
		return;
	/* Assmeble packet */
	silc_buffer_format(packet,
			   SILC_STR_UI32_STRING(SILCPURPLE_WB_MIME),
			   SILC_STR_UI_CHAR(SILCPURPLE_WB_DRAW),
			   SILC_STR_UI_SHORT(wbs->width),
			   SILC_STR_UI_SHORT(wbs->height),
			   SILC_STR_UI_INT(wbs->brush_color),
			   SILC_STR_UI_SHORT(wbs->brush_size),
			   SILC_STR_END);
	silc_buffer_pull(packet, SILCPURPLE_WB_HEADER);
	for (list = draw_list; list; list = list->next) {
		silc_buffer_format(packet,
				   SILC_STR_UI_INT(GPOINTER_TO_INT(list->data)),
				   SILC_STR_END);
		silc_buffer_pull(packet, 4);
	}
	/* Send the message */
	if (wbs->type == 0) {
		/* Private message */
		silc_client_send_private_message(sg->client, sg->conn,
						 wbs->u.client,
						 SILC_MESSAGE_FLAG_DATA,
						 packet->head, len, TRUE);
	} else if (wbs->type == 1) {
		/* Channel message. Channel private keys are not supported. */
		silc_client_send_channel_message(sg->client, sg->conn,
						 wbs->u.channel, NULL,
						 SILC_MESSAGE_FLAG_DATA,
						 packet->head, len, TRUE);
	}
	silc_buffer_free(packet);
}
