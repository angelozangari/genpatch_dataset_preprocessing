}
static void zephyr_action_get_subs_from_server(PurplePluginAction *action)
{
	PurpleConnection *gc = (PurpleConnection *) action->context;
	zephyr_account *zephyr = gc->proto_data;
	gchar *title;
	int retval, nsubs, one,i;
	ZSubscription_t subs;
	if (use_zeph02(zephyr)) {
		GString* subout = g_string_new("Subscription list<br>");
		title = g_strdup_printf("Server subscriptions for %s", zephyr->username);
		if (zephyr->port == 0) {
			purple_debug_error("zephyr", "error while retrieving port\n");
			return;
		}
		if ((retval = ZRetrieveSubscriptions(zephyr->port,&nsubs)) != ZERR_NONE) {
			/* XXX better error handling */
			purple_debug_error("zephyr", "error while retrieving subscriptions from server\n");
			return;
		}
		for(i=0;i<nsubs;i++) {
			one = 1;
			if ((retval = ZGetSubscriptions(&subs,&one)) != ZERR_NONE) {
				/* XXX better error handling */
				purple_debug_error("zephyr", "error while retrieving individual subscription\n");
				return;
			}
			g_string_append_printf(subout, "Class %s Instance %s Recipient %s<br>",
					       subs.zsub_class, subs.zsub_classinst,
					       subs.zsub_recipient);
		}
		purple_notify_formatted(gc, title, title, NULL,  subout->str, NULL, NULL);
	} else {
		/* XXX fix */
		purple_notify_error(gc,"","tzc doesn't support this action",NULL);
	}
}
