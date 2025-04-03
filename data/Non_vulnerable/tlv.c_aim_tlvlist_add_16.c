 */
int aim_tlvlist_add_16(GSList **list, const guint16 type, const guint16 value)
{
	guint8 v16[2];
	(void)aimutil_put16(v16, value);
	return aim_tlvlist_add_raw(list, type, 2, v16);
}
