 */
aim_tlv_t *aim_tlv_gettlv(GSList *list, const guint16 type, const int nth)
{
	GSList *cur;
	aim_tlv_t *tlv;
	int i;
	for (cur = list, i = 0; cur != NULL; cur = cur->next) {
		tlv = cur->data;
		if (tlv->type == type)
			i++;
		if (i >= nth)
			return tlv;
	}
	return NULL;
}
