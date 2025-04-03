}
static void process_anyone(PurpleConnection *gc)
{
	zephyr_account *zephyr = purple_connection_get_protocol_data(gc);
	FILE *fd;
	gchar buff[BUFSIZ], *filename;
	PurpleGroup *g;
	PurpleBuddy *b;
	if (!(g = purple_find_group(_("Anyone")))) {
		g = purple_group_new(_("Anyone"));
		purple_blist_add_group(g, NULL);
	}
	filename = g_strconcat(purple_home_dir(), "/.anyone", NULL);
	if ((fd = g_fopen(filename, "r")) != NULL) {
		while (fgets(buff, BUFSIZ, fd)) {
			strip_comments(buff);
			if (buff[0]) {
				if (!purple_find_buddy(gc->account, buff)) {
					char *stripped_user = zephyr_strip_local_realm(zephyr,buff);
					purple_debug_info("zephyr","stripped_user %s\n",stripped_user);
					if (!purple_find_buddy(gc->account,stripped_user)){
						b = purple_buddy_new(gc->account, stripped_user, NULL);
						purple_blist_add_buddy(b, NULL, g, NULL);
					}
					g_free(stripped_user);
				}
			}
		}
		fclose(fd);
	}
	g_free(filename);
}
