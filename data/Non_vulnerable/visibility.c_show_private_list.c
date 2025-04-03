static void
show_private_list(PurplePluginAction *action, guint16 list_type, const gchar *title, const gchar *list_description, const gchar *menu_action_name)
{
	PurpleConnection *gc = (PurpleConnection *) action->context;
	OscarData *od = purple_connection_get_protocol_data(gc);
	PurpleAccount *account = purple_connection_get_account(gc);
	GSList *buddies, *filtered_buddies, *cur;
	gchar *text, *secondary;
	buddies = purple_find_buddies(account, NULL);
	filtered_buddies = NULL;
	for (cur = buddies; cur != NULL; cur = cur->next) {
		PurpleBuddy *buddy;
		const gchar *bname;
		buddy = cur->data;
		bname = purple_buddy_get_name(buddy);
		if (aim_ssi_itemlist_finditem(od->ssi.local, NULL, bname, list_type)) {
			filtered_buddies = g_slist_prepend(filtered_buddies, buddy);
		}
	}
	g_slist_free(buddies);
	filtered_buddies = g_slist_reverse(filtered_buddies);
	text = oscar_format_buddies(filtered_buddies, _("you have no buddies on this list"));
	g_slist_free(filtered_buddies);
	secondary = g_strdup_printf(_("You can add a buddy to this list "
					"by right-clicking on them and "
					"selecting \"%s\""), menu_action_name);
	purple_notify_formatted(gc, title, list_description, secondary, text, NULL, NULL);
	g_free(secondary);
	g_free(text);
}
