 */
int aim_tlvlist_add_raw(GSList **list, const guint16 type, const guint16 length, const guint8 *value)
{
	aim_tlv_t *tlv;
	if (list == NULL)
		return 0;
	tlv = createtlv(type, length, NULL);
	if (tlv->length > 0)
		tlv->value = g_memdup(value, length);
	*list = g_slist_append(*list, tlv);
	return tlv->length;
}
