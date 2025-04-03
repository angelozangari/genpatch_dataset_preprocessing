}
char *local_zephyr_normalize(zephyr_account *zephyr,const char *orig)
{
	/*
	   Basically the inverse of zephyr_strip_local_realm
	*/
	char* buf;
	if (!g_ascii_strcasecmp(orig, "")) {
		return g_strdup("");
	}
	if (strchr(orig,'@')) {
		buf = g_strdup_printf("%s",orig);
	} else {
		buf = g_strdup_printf("%s@%s",orig,zephyr->realm);
	}
	return buf;
}
