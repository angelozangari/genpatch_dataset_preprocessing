}
static void zephyr_join_chat(PurpleConnection * gc, GHashTable * data)
{
	/*	ZSubscription_t sub; */
	zephyr_triple *zt1, *zt2;
	const char *classname;
	const char *instname;
	const char *recip;
	zephyr_account *zephyr=gc->proto_data;
	classname = g_hash_table_lookup(data, "class");
	instname = g_hash_table_lookup(data, "instance");
	recip = g_hash_table_lookup(data, "recipient");
	if (!classname)
		return;
	if (!g_ascii_strcasecmp(classname,"%host%"))
		classname = g_strdup(zephyr->ourhost);
	if (!g_ascii_strcasecmp(classname,"%canon%"))
		classname = g_strdup(zephyr->ourhostcanon);
	if (!instname || !strlen(instname))
		instname = "*";
	if (!g_ascii_strcasecmp(instname,"%host%"))
		instname = g_strdup(zephyr->ourhost);
	if (!g_ascii_strcasecmp(instname,"%canon%"))
		instname = g_strdup(zephyr->ourhostcanon);
	if (!recip || (*recip == '*'))
		recip = "";
	if (!g_ascii_strcasecmp(recip, "%me%"))
		recip = zephyr->username;
	zt1 = new_triple(zephyr,classname, instname, recip);
	zt2 = find_sub_by_triple(zephyr,zt1);
	if (zt2) {
		free_triple(zt1);
		if (!zt2->open) {
			if (!g_ascii_strcasecmp(instname,"*"))
				instname = "PERSONAL";
			serv_got_joined_chat(gc, zt2->id, zt2->name);
			zephyr_chat_set_topic(gc,zt2->id,instname);
			zt2->open = TRUE;
		}
		return;
	}
	/*	sub.zsub_class = zt1->class;
		sub.zsub_classinst = zt1->instance;
		sub.zsub_recipient = zt1->recipient; */
	if (zephyr_subscribe_to(zephyr,zt1->class,zt1->instance,zt1->recipient,NULL) != ZERR_NONE) {
		/* XXX output better subscription information */
		zephyr_subscribe_failed(gc,zt1->class,zt1->instance,zt1->recipient,NULL);
		free_triple(zt1);
		return;
	}
	zephyr->subscrips = g_slist_append(zephyr->subscrips, zt1);
	zt1->open = TRUE;
	serv_got_joined_chat(gc, zt1->id, zt1->name);
	if (!g_ascii_strcasecmp(instname,"*"))
		instname = "PERSONAL";
	zephyr_chat_set_topic(gc,zt1->id,instname);
}
