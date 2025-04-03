 */
int aim_tlvlist_size(GSList *list)
{
	GSList *cur;
	int size;
	if (list == NULL)
		return 0;
	for (cur = list, size = 0; cur; cur = cur->next)
		size += (4 + ((aim_tlv_t *)cur->data)->length);
	return size;
}
