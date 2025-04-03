}
static void zephyr_zloc(PurpleConnection *gc, const char *who)
{
	ZAsyncLocateData_t ald;
	zephyr_account *zephyr = gc->proto_data;
	gchar* normalized_who = local_zephyr_normalize(zephyr,who);
	if (use_zeph02(zephyr)) {
		if (ZRequestLocations(normalized_who, &ald, UNACKED, ZAUTH) == ZERR_NONE) {
			zephyr->pending_zloc_names = g_list_append(zephyr->pending_zloc_names,
								   g_strdup(normalized_who));
		} else {
			/* XXX deal with errors somehow */
		}
	} else if (use_tzc(zephyr)) {
		size_t len;
		size_t result;
		char* zlocstr = g_strdup_printf("((tzcfodder . zlocate) \"%s\")\n",normalized_who);
		zephyr->pending_zloc_names = g_list_append(zephyr->pending_zloc_names, g_strdup(normalized_who));
		len = strlen(zlocstr);
		result = write(zephyr->totzc[ZEPHYR_FD_WRITE],zlocstr,len);
		if (result != len) {
			purple_debug_error("zephyr", "Unable to write a message: %s\n", g_strerror(errno));
		}
		g_free(zlocstr);
	}
}
