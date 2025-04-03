 */
GSList *aim_tlvlist_readlen(ByteStream *bs, guint16 len)
{
	GSList *list = NULL;
	while ((byte_stream_bytes_left(bs) > 0) && (len > 0)) {
		list = aim_tlv_read(list, bs);
		if (list == NULL)
			return NULL;
		len -= 2 + 2 + ((aim_tlv_t *)list->data)->length;
	}
	return g_slist_reverse(list);
}
