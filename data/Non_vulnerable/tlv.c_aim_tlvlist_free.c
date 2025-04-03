 */
void aim_tlvlist_free(GSList *list)
{
	while (list != NULL)
	{
		freetlv(list->data);
		list = g_slist_delete_link(list, list);
	}
}
