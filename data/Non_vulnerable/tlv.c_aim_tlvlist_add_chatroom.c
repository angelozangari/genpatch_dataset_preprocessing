 */
int aim_tlvlist_add_chatroom(GSList **list, guint16 type, guint16 exchange, const char *roomname, guint16 instance)
{
	int len;
	ByteStream bs;
	byte_stream_new(&bs, 2 + 1 + strlen(roomname) + 2);
	byte_stream_put16(&bs, exchange);
	byte_stream_put8(&bs, strlen(roomname));
	byte_stream_putstr(&bs, roomname);
	byte_stream_put16(&bs, instance);
	len = aim_tlvlist_add_raw(list, type, byte_stream_curpos(&bs), bs.data);
	byte_stream_destroy(&bs);
	return len;
}
