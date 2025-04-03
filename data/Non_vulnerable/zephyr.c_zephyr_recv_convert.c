*/
static gchar *zephyr_recv_convert(PurpleConnection *gc, gchar *string)
{
	gchar *utf8;
	GError *err = NULL;
	zephyr_account *zephyr = gc->proto_data;
	if (g_utf8_validate(string, -1, NULL)) {
		return g_strdup(string);
	} else {
		utf8 = g_convert(string, -1, "UTF-8", zephyr->encoding, NULL, NULL, &err);
		if (err) {
			purple_debug_error("zephyr", "recv conversion error: %s\n", err->message);
			utf8 = g_strdup(_("(There was an error converting this message.	 Check the 'Encoding' option in the Account Editor)"));
			g_error_free(err);
		}
		return utf8;
	}
}
