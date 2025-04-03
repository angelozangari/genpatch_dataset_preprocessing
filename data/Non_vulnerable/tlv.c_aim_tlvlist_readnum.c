 */
GSList *aim_tlvlist_readnum(ByteStream *bs, guint16 num)
{
	GSList *list = NULL;
	while ((byte_stream_bytes_left(bs) > 0) && (num != 0)) {
		list = aim_tlv_read(list, bs);
		if (list == NULL)
			return NULL;
		num--;
	}
	return g_slist_reverse(list);
}
