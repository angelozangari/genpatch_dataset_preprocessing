 */
int aim_tlvlist_add_noval(GSList **list, const guint16 type)
{
	return aim_tlvlist_add_raw(list, type, 0, NULL);
}
