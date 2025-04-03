}
static GList *zephyr_status_types(PurpleAccount *account)
{
	PurpleStatusType *type;
	GList *types = NULL;
	/* zephyr has several exposures
	   NONE (where you are hidden, and zephyrs to you are in practice silently dropped -- yes this is wrong)
	   OPSTAFF "hidden"
	   REALM-VISIBLE visible to people in local realm
	   REALM-ANNOUNCED REALM-VISIBLE+ plus your logins/logouts are announced to <login,username,*>
	   NET-VISIBLE REALM-ANNOUNCED, plus visible to people in foreign realm
	   NET-ANNOUNCED NET-VISIBLE, plus logins/logouts are announced	 to <login,username,*>
	   Online will set the user to the exposure they have in their options (defaulting to REALM-VISIBLE),
	   Hidden, will set the user's exposure to OPSTAFF
	   Away won't change their exposure but will set an auto away message (for IMs only)
	*/
	type = purple_status_type_new(PURPLE_STATUS_AVAILABLE, NULL, NULL, TRUE);
	types = g_list_append(types,type);
	type = purple_status_type_new(PURPLE_STATUS_INVISIBLE, NULL, NULL, TRUE);
	types = g_list_append(types,type);
	type = purple_status_type_new_with_attrs(
					       PURPLE_STATUS_AWAY, NULL, NULL, TRUE, TRUE, FALSE,
					       "message", _("Message"), purple_value_new(PURPLE_TYPE_STRING),
					       NULL);
	types = g_list_append(types, type);
	type = purple_status_type_new(PURPLE_STATUS_OFFLINE, NULL, NULL, TRUE);
	types = g_list_append(types,type);
	return types;
}
