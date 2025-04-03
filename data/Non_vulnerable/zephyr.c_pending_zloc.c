}
static gboolean pending_zloc(zephyr_account *zephyr, const char *who)
{
	GList *curr;
	for (curr = zephyr->pending_zloc_names; curr != NULL; curr = curr->next) {
		char* normalized_who = local_zephyr_normalize(zephyr,who);
		if (!g_ascii_strcasecmp(normalized_who, (char *)curr->data)) {
			g_free((char *)curr->data);
			zephyr->pending_zloc_names = g_list_remove(zephyr->pending_zloc_names, curr->data);
			return TRUE;
		}
	}
	return FALSE;
}
