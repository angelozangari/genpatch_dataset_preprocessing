 */
int aim_tlv_getlength(GSList *list, const guint16 type, const int nth)
{
	aim_tlv_t *tlv;
	tlv = aim_tlv_gettlv(list, type, nth);
	if (tlv == NULL)
		return -1;
	return tlv->length;
}
