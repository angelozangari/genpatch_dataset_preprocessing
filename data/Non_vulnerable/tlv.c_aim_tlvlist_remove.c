 */
void aim_tlvlist_remove(GSList **list, const guint16 type)
{
	GSList *cur, *next;
	aim_tlv_t *tlv;
	if (list == NULL || *list == NULL)
		return;
	cur = *list;
	while (cur != NULL)
	{
		tlv = cur->data;
		next = cur->next;
		if (tlv->type == type)
		{
			/* Delete this TLV */
			*list = g_slist_delete_link(*list, cur);
			g_free(tlv->value);
			g_free(tlv);
		}
		cur = next;
	}
}
