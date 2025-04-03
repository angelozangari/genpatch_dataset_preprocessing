PurpleMenuAction *
create_visibility_menu_item(OscarData *od, const char *bname)
{
	PurpleAccount *account = purple_connection_get_account(od->gc);
	gboolean invisible = purple_account_is_status_active(account, OSCAR_STATUS_ID_INVISIBLE);
	gboolean on_list = is_buddy_on_list(od, bname);
	const gchar *label;
	if (invisible) {
		label = on_list ? _(DONT_APPEAR_ONLINE) : _(APPEAR_ONLINE);
	} else {
		label = on_list ? _(DONT_APPEAR_OFFLINE) : _(APPEAR_OFFLINE);
	}
	return purple_menu_action_new(label, PURPLE_CALLBACK(visibility_cb), NULL, NULL);
}
