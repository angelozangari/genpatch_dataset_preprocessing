 */
int aim_tlvlist_replace_str(GSList **list, const guint16 type, const char *str)
{
	return aim_tlvlist_replace_raw(list, type, strlen(str), (const guchar *)str);
}
