static GSList *
aim_tlv_read(GSList *list, ByteStream *bs)
{
	guint16 type, length;
	aim_tlv_t *tlv;
	type = byte_stream_get16(bs);
	length = byte_stream_get16(bs);
	if (length > byte_stream_bytes_left(bs)) {
		aim_tlvlist_free(list);
		return NULL;
	}
	tlv = createtlv(type, length, NULL);
	if (tlv->length > 0) {
		tlv->value = byte_stream_getraw(bs, length);
		if (!tlv->value) {
			freetlv(tlv);
			aim_tlvlist_free(list);
			return NULL;
		}
	}
	return g_slist_prepend(list, tlv);
}
