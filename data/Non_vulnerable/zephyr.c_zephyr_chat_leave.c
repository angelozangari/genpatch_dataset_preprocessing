}
static void zephyr_chat_leave(PurpleConnection * gc, int id)
{
	zephyr_triple *zt;
	zephyr_account *zephyr = gc->proto_data;
	zt = find_sub_by_id(zephyr,id);
	if (zt) {
		zt->open = FALSE;
		zt->id = ++(zephyr->last_id);
	}
}
