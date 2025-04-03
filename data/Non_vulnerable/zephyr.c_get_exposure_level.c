#endif /* WIN32 */
static char *get_exposure_level(void)
{
	/* XXX add real error reporting */
	char *exposure = ZGetVariable("exposure");
	if (!exposure)
		return EXPOSE_REALMVIS;
	if (!g_ascii_strcasecmp(exposure, EXPOSE_NONE))
		return EXPOSE_NONE;
	if (!g_ascii_strcasecmp(exposure, EXPOSE_OPSTAFF))
		return EXPOSE_OPSTAFF;
	if (!g_ascii_strcasecmp(exposure, EXPOSE_REALMANN))
		return EXPOSE_REALMANN;
	if (!g_ascii_strcasecmp(exposure, EXPOSE_NETVIS))
		return EXPOSE_NETVIS;
	if (!g_ascii_strcasecmp(exposure, EXPOSE_NETANN))
		return EXPOSE_NETANN;
	return EXPOSE_REALMVIS;
}
