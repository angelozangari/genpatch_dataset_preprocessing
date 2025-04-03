static void
visibility_cb(PurpleBlistNode *node, gpointer whatever)
{
	PurpleBuddy *buddy = PURPLE_BUDDY(node);
	const char* bname = purple_buddy_get_name(buddy);
	OscarData *od = purple_connection_get_protocol_data(purple_account_get_connection(purple_buddy_get_account(buddy)));
	guint16 list_type = get_buddy_list_type(od);
	if (!is_buddy_on_list(od, bname)) {
		aim_ssi_add_to_private_list(od, bname, list_type);
	} else {
		aim_ssi_del_from_private_list(od, bname, list_type);
	}
}
