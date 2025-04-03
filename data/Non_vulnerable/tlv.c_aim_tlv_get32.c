 */
guint32 aim_tlv_get32(GSList *list, const guint16 type, const int nth)
{
	aim_tlv_t *tlv;
	tlv = aim_tlv_gettlv(list, type, nth);
	if (tlv == NULL)
		return 0; /* erm */
	return aimutil_get32(tlv->value);
}
