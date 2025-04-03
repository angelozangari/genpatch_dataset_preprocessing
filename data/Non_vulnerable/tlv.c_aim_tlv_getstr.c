 */
char *aim_tlv_getstr(GSList *list, const guint16 type, const int nth)
{
	aim_tlv_t *tlv;
	tlv = aim_tlv_gettlv(list, type, nth);
	if (tlv == NULL)
		return NULL;
	return aim_tlv_getvalue_as_string(tlv);
}
