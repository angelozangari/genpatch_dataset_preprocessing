#ifdef WIN32
static gint check_loc(gpointer data)
{
	GSList *buddies;
	ZLocations_t locations;
	PurpleConnection *gc = data;
	zephyr_account *zephyr = gc->proto_data;
	PurpleAccount *account = purple_connection_get_account(gc);
	int numlocs;
	int one = 1;
	for (buddies = purple_find_buddies(account, NULL); buddies;
			buddies = g_slist_delete_link(buddies, buddies)) {
		PurpleBuddy *b = buddies->data;
		char *chk;
		const char *bname = purple_buddy_get_name(b);
		chk = local_zephyr_normalize(bname);
		ZLocateUser(chk,&numlocs, ZAUTH);
		if (numlocs) {
			int i;
			for(i=0;i<numlocs;i++) {
				ZGetLocations(&locations,&one);
				serv_got_update(zgc,bname,1,0,0,0,0);
			}
		}
	}
	return TRUE;
}
#else
static gint check_loc(gpointer data)
{
	GSList *buddies;
	ZAsyncLocateData_t ald;
	PurpleConnection *gc = (PurpleConnection *)data;
	zephyr_account *zephyr = gc->proto_data;
	PurpleAccount *account = purple_connection_get_account(gc);
	if (use_zeph02(zephyr)) {
		ald.user = NULL;
		memset(&(ald.uid), 0, sizeof(ZUnique_Id_t));
		ald.version = NULL;
	}
	for (buddies = purple_find_buddies(account, NULL); buddies;
			buddies = g_slist_delete_link(buddies, buddies)) {
		PurpleBuddy *b = buddies->data;
		const char *chk;
		const char *name = purple_buddy_get_name(b);
		chk = local_zephyr_normalize(zephyr,name);
		purple_debug_info("zephyr","chk: %s b->name %s\n",chk,name);
		/* XXX add real error reporting */
		/* doesn't matter if this fails or not; we'll just move on to the next one */
		if (use_zeph02(zephyr)) {
#ifdef WIN32
			int numlocs;
			int one=1;
			ZLocateUser(chk,&numlocs,ZAUTH);
			if (numlocs) {
				int i;
				for(i=0;i<numlocs;i++) {
					ZGetLocations(&locations,&one);
					if (nlocs>0)
						purple_prpl_got_user_status(account,name,"available",NULL);
					else
						purple_prpl_got_user_status(account,name,"offline",NULL);
				}
			}
#else
			ZRequestLocations(chk, &ald, UNACKED, ZAUTH);
			g_free(ald.user);
			g_free(ald.version);
#endif /* WIN32 */
		} else
			if (use_tzc(zephyr)) {
				gchar *zlocstr = g_strdup_printf("((tzcfodder . zlocate) \"%s\")\n",chk);
				size_t len = strlen(zlocstr);
				size_t result = write(zephyr->totzc[ZEPHYR_FD_WRITE],zlocstr,len);
				if (result != len) {
					purple_debug_error("zephyr", "Unable to write a message: %s\n", g_strerror(errno));
				}
				g_free(zlocstr);
			}
	}
	return TRUE;
}
