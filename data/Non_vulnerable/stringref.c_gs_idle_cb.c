}
static gboolean gs_idle_cb(gpointer data)
{
	PurpleStringref *ref;
	GList *del;
	while (gclist != NULL) {
		ref = gclist->data;
		if (REFCOUNT(ref->ref) == 0) {
			stringref_free(ref);
		}
		del = gclist;
		gclist = gclist->next;
		g_list_free_1(del);
	}
	return FALSE;
}
