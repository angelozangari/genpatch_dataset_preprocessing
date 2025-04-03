static aim_tlv_t *
createtlv(guint16 type, guint16 length, guint8 *value)
{
	aim_tlv_t *ret;
	ret = g_new(aim_tlv_t, 1);
	ret->type = type;
	ret->length = length;
	ret->value = value;
	return ret;
}
