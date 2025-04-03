 */
GSList *aim_tlvlist_read(ByteStream *bs)
{
	GSList *list = NULL;
	while (byte_stream_bytes_left(bs) > 0) {
		list = aim_tlv_read(list, bs);
		if (list == NULL)
			return NULL;
	}
	return g_slist_reverse(list);
}
