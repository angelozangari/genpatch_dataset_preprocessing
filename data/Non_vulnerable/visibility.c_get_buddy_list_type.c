static guint16
get_buddy_list_type(OscarData *od)
{
	PurpleAccount *account = purple_connection_get_account(od->gc);
	return purple_account_is_status_active(account, OSCAR_STATUS_ID_INVISIBLE) ? AIM_SSI_TYPE_PERMIT : AIM_SSI_TYPE_DENY;
}
