}
static char* normalize_zephyr_exposure(const char* exposure) {
	char *exp2 = g_strstrip(g_ascii_strup(exposure,-1));
	if (!exp2)
		return EXPOSE_REALMVIS;
	if (!g_ascii_strcasecmp(exp2, EXPOSE_NONE))
		return EXPOSE_NONE;
	if (!g_ascii_strcasecmp(exp2, EXPOSE_OPSTAFF))
		return EXPOSE_OPSTAFF;
	if (!g_ascii_strcasecmp(exp2, EXPOSE_REALMANN))
		return EXPOSE_REALMANN;
	if (!g_ascii_strcasecmp(exp2, EXPOSE_NETVIS))
		return EXPOSE_NETVIS;
	if (!g_ascii_strcasecmp(exp2, EXPOSE_NETANN))
		return EXPOSE_NETANN;
	return EXPOSE_REALMVIS;
}
