char *
aim_tlv_getvalue_as_string(aim_tlv_t *tlv)
{
	char *ret;
	ret = g_malloc(tlv->length + 1);
	memcpy(ret, tlv->value, tlv->length);
	ret[tlv->length] = '\0';
	return ret;
}
