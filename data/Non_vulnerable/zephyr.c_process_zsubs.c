}
static void process_zsubs(zephyr_account *zephyr)
{
	/* Loads zephyr chats "(subscriptions) from ~/.zephyr.subs, and
	   registers (subscribes to) them on the server */
	/* XXX deal with unsubscriptions */
	/* XXX deal with punts */
	FILE *f;
	gchar *fname;
	gchar buff[BUFSIZ];
	fname = g_strdup_printf("%s/.zephyr.subs", purple_home_dir());
	f = g_fopen(fname, "r");
	if (f) {
		char **triple;
		char *recip;
		char *z_class;
		char *z_instance;
		char *z_galaxy = NULL;
		while (fgets(buff, BUFSIZ, f)) {
			strip_comments(buff);
			if (buff[0]) {
				triple = g_strsplit(buff, ",", 3);
				if (triple[0] && triple[1]) {
					char *tmp = g_strdup_printf("%s", zephyr->username);
					char *atptr;
					z_class = triple[0];
					z_instance = triple[1];
					if (triple[2] == NULL) {
						recip = g_malloc0(1);
					} else if (!g_ascii_strcasecmp(triple[2], "%me%")) {
						recip = g_strdup_printf("%s", zephyr->username);
					} else if (!g_ascii_strcasecmp(triple[2], "*")) {
						/* wildcard
						 * form of class,instance,* */
						recip = g_malloc0(1);
					} else if (!g_ascii_strcasecmp(triple[2], tmp)) {
						/* form of class,instance,aatharuv@ATHENA.MIT.EDU */
						recip = g_strdup(triple[2]);
					} else if ((atptr = strchr(triple[2], '@')) != NULL) {
						/* form of class,instance,*@ANDREW.CMU.EDU
						 * class,instance,@ANDREW.CMU.EDU
						 * If realm is local realm, blank recipient, else
						 * @REALM-NAME
						 */
						char *realmat = g_strdup_printf("@%s",zephyr->realm);
						if (!g_ascii_strcasecmp(atptr, realmat))
							recip = g_malloc0(1);
						else
							recip = g_strdup(atptr);
						g_free(realmat);
					} else {
						recip = g_strdup(triple[2]);
					}
					g_free(tmp);
					if (!g_ascii_strcasecmp(triple[0],"%host%")) {
						z_class = g_strdup(zephyr->ourhost);
					} else if (!g_ascii_strcasecmp(triple[0],"%canon%")) {
						z_class = g_strdup(zephyr->ourhostcanon);
					} else {
						z_class = g_strdup(triple[0]);
					}
					if (!g_ascii_strcasecmp(triple[1],"%host%")) {
						z_instance = g_strdup(zephyr->ourhost);
					} else if (!g_ascii_strcasecmp(triple[1],"%canon%")) {
						z_instance = g_strdup(zephyr->ourhostcanon);
					} else {
						z_instance = g_strdup(triple[1]);
					}
					/* There should be some sort of error report listing classes that couldn't be subbed to.
					   Not important right now though */
					if (zephyr_subscribe_to(zephyr,z_class, z_instance, recip,z_galaxy) != ZERR_NONE) {
						purple_debug_error("zephyr", "Couldn't subscribe to %s, %s, %s\n", z_class,z_instance,recip);
					}
					zephyr->subscrips = g_slist_append(zephyr->subscrips, new_triple(zephyr,z_class,z_instance,recip));
					/*					  g_hash_table_destroy(sub_hash_table); */
					g_free(z_instance);
					g_free(z_class);
					g_free(recip);
				}
				g_strfreev(triple);
			}
		}
		fclose(f);
	}
}
