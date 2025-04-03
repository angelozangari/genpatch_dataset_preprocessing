 */
int aim_tlvlist_replace_8(GSList **list, const guint16 type, const guint8 value)
{
	guint8 v8[1];
	(void)aimutil_put8(v8, value);
	return aim_tlvlist_replace_raw(list, type, 1, v8);
}
