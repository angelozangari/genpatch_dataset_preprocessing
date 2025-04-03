 */
int aim_tlvlist_write(ByteStream *bs, GSList **list)
{
	size_t goodbuflen;
	GSList *cur;
	aim_tlv_t *tlv;
	/* do an initial run to test total length */
	goodbuflen = aim_tlvlist_size(*list);
	if (goodbuflen > byte_stream_bytes_left(bs))
		return 0; /* not enough buffer */
	/* do the real write-out */
	for (cur = *list; cur; cur = cur->next) {
		tlv = cur->data;
		byte_stream_put16(bs, tlv->type);
		byte_stream_put16(bs, tlv->length);
		if (tlv->length > 0)
			byte_stream_putraw(bs, tlv->value, tlv->length);
	}
	return 1; /* TODO: This is a nonsensical return */
}
