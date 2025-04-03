 */
int aim_tlvlist_add_32(GSList **list, const guint16 type, const guint32 value)
{
	guint8 v32[4];
	(void)aimutil_put32(v32, value);
	return aim_tlvlist_add_raw(list, type, 4, v32);
}
