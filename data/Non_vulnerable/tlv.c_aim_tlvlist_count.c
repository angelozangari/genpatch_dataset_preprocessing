 */
int aim_tlvlist_count(GSList *list)
{
	GSList *cur;
	int count;
	if (list == NULL)
		return 0;
	for (cur = list, count = 0; cur; cur = cur->next)
		count++;
	return count;
}
