 */
int aim_tlvlist_add_str(GSList **list, const guint16 type, const char *value)
{
	return aim_tlvlist_add_raw(list, type, strlen(value), (guint8 *)value);
}
