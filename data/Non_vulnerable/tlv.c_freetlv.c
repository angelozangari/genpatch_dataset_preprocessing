static void
freetlv(aim_tlv_t *oldtlv)
{
	g_free(oldtlv->value);
	g_free(oldtlv);
}
