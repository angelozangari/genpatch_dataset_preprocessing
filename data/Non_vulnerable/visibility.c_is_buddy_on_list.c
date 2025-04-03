static gboolean
is_buddy_on_list(OscarData *od, const char *bname)
{
	return aim_ssi_itemlist_finditem(od->ssi.local, NULL, bname, get_buddy_list_type(od)) != NULL;
}
