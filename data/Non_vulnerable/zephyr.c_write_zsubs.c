}
static void write_zsubs(zephyr_account *zephyr)
{
	/* Exports subscription (chat) list back to
	 * .zephyr.subs
	 * XXX deal with %host%, %canon%, unsubscriptions, and negative subscriptions (punts?)
	 */
	GSList *s = zephyr->subscrips;
	zephyr_triple *zt;
	FILE *fd;
	char *fname;
	char **triple;
	fname = g_strdup_printf("%s/.zephyr.subs", purple_home_dir());
	fd = g_fopen(fname, "w");
	if (!fd) {
		g_free(fname);
		return;
	}
	while (s) {
		char *zclass, *zinst, *zrecip;
		zt = s->data;
		triple = g_strsplit(zt->name, ",", 3);
		/* deal with classes */
		if (!g_ascii_strcasecmp(triple[0],zephyr->ourhost)) {
			zclass = g_strdup("%host%");
		} else if (!g_ascii_strcasecmp(triple[0],zephyr->ourhostcanon)) {
			zclass = g_strdup("%canon%");
		} else {
			zclass = g_strdup(triple[0]);
		}
		/* deal with instances */
		if (!g_ascii_strcasecmp(triple[1],zephyr->ourhost)) {
			zinst = g_strdup("%host%");
		} else if (!g_ascii_strcasecmp(triple[1],zephyr->ourhostcanon)) {
			zinst = g_strdup("%canon%");;
		} else {
			zinst = g_strdup(triple[1]);
		}
		/* deal with recipients */
		if (triple[2] == NULL) {
			zrecip = g_strdup("*");
		} else if (!g_ascii_strcasecmp(triple[2],"")){
			zrecip = g_strdup("*");
		} else if (!g_ascii_strcasecmp(triple[2], zephyr->username)) {
			zrecip = g_strdup("%me%");
		} else {
			zrecip = g_strdup(triple[2]);
		}
		fprintf(fd, "%s,%s,%s\n",zclass,zinst,zrecip);
		g_free(zclass);
		g_free(zinst);
		g_free(zrecip);
		g_free(triple);
		s = s->next;
	}
	g_free(fname);
	fclose(fd);
}
