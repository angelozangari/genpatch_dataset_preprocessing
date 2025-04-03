}
static char *timestamp_cb_common(PurpleConversation *conv,
                                 time_t t,
                                 gboolean show_date,
                                 const char *force,
                                 const char *dates,
								 gboolean parens)
{
	struct tm *tm;
	g_return_val_if_fail(dates != NULL, NULL);
	tm = localtime(&t);
	if (show_date ||
	    !strcmp(dates, "always") ||
	    (conv != NULL && purple_conversation_get_type(conv) == PURPLE_CONV_TYPE_CHAT && !strcmp(dates, "chats")))
	{
		if (g_str_equal(force, "force24"))
			return g_strdup_printf("%s%s%s", parens ? "(" : "", purple_utf8_strftime("%Y-%m-%d %H:%M:%S", tm), parens ? ")" : "");
		else if (g_str_equal(force, "force12")) {
			char *date = g_strdup_printf("%s", purple_utf8_strftime("%Y-%m-%d ", tm));
			char *remtime = g_strdup_printf("%s", purple_utf8_strftime(":%M:%S %p", tm));
			const char *hour = format_12hour_hour(tm);
			char *output;
			output = g_strdup_printf("%s%s%s%s%s",
			                         parens ? "(" : "", date,
									 hour, remtime, parens ? ")" : "");
			g_free(date);
			g_free(remtime);
			return output;
		} else
			return g_strdup_printf("%s%s%s", parens ? "(" : "", purple_date_format_long(tm), parens ? ")" : "");
	}
	if (g_str_equal(force, "force24"))
		return g_strdup_printf("%s%s%s", parens ? "(" : "", purple_utf8_strftime("%H:%M:%S", tm), parens ? ")" : "");
	else if (g_str_equal(force, "force12")) {
		const char *hour = format_12hour_hour(tm);
		char *remtime = g_strdup_printf("%s", purple_utf8_strftime(":%M:%S %p", tm));
		char *output = g_strdup_printf("%s%s%s%s", parens ? "(" : "", hour, remtime, parens ? ")" : "");
		g_free(remtime);
		return output;
	}
	return NULL;
}
