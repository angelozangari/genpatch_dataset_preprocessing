 */
int aim_tlvlist_replace_noval(GSList **list, const guint16 type)
{
	return aim_tlvlist_replace_raw(list, type, 0, NULL);
}
