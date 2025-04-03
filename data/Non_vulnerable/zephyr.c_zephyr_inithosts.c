}
static void zephyr_inithosts(zephyr_account *zephyr)
{
	/* XXX This code may not be Win32 clean */
	struct hostent *hent;
	if (gethostname(zephyr->ourhost, sizeof(zephyr->ourhost)) == -1) {
		purple_debug_error("zephyr", "unable to retrieve hostname, %%host%% and %%canon%% will be wrong in subscriptions and have been set to unknown\n");
		g_strlcpy(zephyr->ourhost, "unknown", sizeof(zephyr->ourhost));
		g_strlcpy(zephyr->ourhostcanon, "unknown", sizeof(zephyr->ourhostcanon));
		return;
	}
	if (!(hent = gethostbyname(zephyr->ourhost))) {
		purple_debug_error("zephyr", "unable to resolve hostname, %%canon%% will be wrong in subscriptions.and has been set to the value of %%host%%, %s\n",zephyr->ourhost);
		g_strlcpy(zephyr->ourhostcanon, zephyr->ourhost, sizeof(zephyr->ourhostcanon));
		return;
	}
	g_strlcpy(zephyr->ourhostcanon, hent->h_name, sizeof(zephyr->ourhostcanon));
	return;
}
