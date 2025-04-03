 */
int aim_tlvlist_replace_raw(GSList **list, const guint16 type, const guint16 length, const guint8 *value)
{
	GSList *cur;
	aim_tlv_t *tlv;
	if (list == NULL)
		return 0;
	for (cur = *list; cur != NULL; cur = cur->next)
	{
		tlv = cur->data;
		if (tlv->type == type)
			break;
	}
	if (cur == NULL)
		/* TLV does not exist, so add a new one */
		return aim_tlvlist_add_raw(list, type, length, value);
	g_free(tlv->value);
	tlv->length = length;
	if (tlv->length > 0) {
		tlv->value = g_memdup(value, length);
	} else
		tlv->value = NULL;
	return tlv->length;
}
