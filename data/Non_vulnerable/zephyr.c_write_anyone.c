}
static void write_anyone(zephyr_account *zephyr)
{
	GSList *buddies;
	char *fname;
	FILE *fd;
	PurpleAccount *account;
	fname = g_strdup_printf("%s/.anyone", purple_home_dir());
	fd = g_fopen(fname, "w");
	if (!fd) {
		g_free(fname);
		return;
	}
	account = zephyr->account;
	for (buddies = purple_find_buddies(account, NULL); buddies;
			buddies = g_slist_delete_link(buddies, buddies)) {
		PurpleBuddy *b = buddies->data;
		gchar *stripped_user = zephyr_strip_local_realm(zephyr, purple_buddy_get_name(b));
		fprintf(fd, "%s\n", stripped_user);
		g_free(stripped_user);
	}
	fclose(fd);
	g_free(fname);
}
