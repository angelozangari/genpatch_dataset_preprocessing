}
static const char *zephyr_normalize(const PurpleAccount *account, const char *who)
{
	static char buf[BUF_LEN];
	PurpleConnection *gc;
	char *tmp;
	gc = purple_account_get_connection(account);
	if (gc == NULL)
		return NULL;
	tmp = local_zephyr_normalize(gc->proto_data, who);
	if (strlen(tmp) >= sizeof(buf)) {
		g_free(tmp);
		return NULL;
	}
	g_strlcpy(buf, tmp, sizeof(buf));
	g_free(tmp);
	return buf;
}
