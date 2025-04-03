}
static gboolean plugin_unload(PurplePlugin *plugin) {
	blist_set_dockable(FALSE);
	blist_set_ontop(FALSE);
	handle = NULL;
	return TRUE;
}
