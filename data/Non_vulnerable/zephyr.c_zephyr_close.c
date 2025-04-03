}
static void zephyr_close(PurpleConnection * gc)
{
	GList *l;
	GSList *s;
	zephyr_account *zephyr = gc->proto_data;
	pid_t tzc_pid = zephyr->tzc_pid;
	l = zephyr->pending_zloc_names;
	while (l) {
		g_free((char *)l->data);
		l = l->next;
	}
	g_list_free(zephyr->pending_zloc_names);
	if (purple_account_get_bool(gc->account, "write_anyone", FALSE))
		write_anyone(zephyr);
	if (purple_account_get_bool(gc->account, "write_zsubs", FALSE))
		write_zsubs(zephyr);
	s = zephyr->subscrips;
	while (s) {
		free_triple((zephyr_triple *) s->data);
		s = s->next;
	}
	g_slist_free(zephyr->subscrips);
	if (zephyr->nottimer)
		purple_timeout_remove(zephyr->nottimer);
	zephyr->nottimer = 0;
	if (zephyr->loctimer)
		purple_timeout_remove(zephyr->loctimer);
	zephyr->loctimer = 0;
	gc = NULL;
	if (use_zeph02(zephyr)) {
		z_call(ZCancelSubscriptions(0));
		z_call(ZUnsetLocation());
		z_call(ZClosePort());
	} else {
		/* assume tzc */
		if (kill(tzc_pid,SIGTERM) == -1) {
			int err=errno;
			if (err==EINVAL) {
				purple_debug_error("zephyr","An invalid signal was specified when killing tzc\n");
			}
			else if (err==ESRCH) {
				purple_debug_error("zephyr","Tzc's pid didn't exist while killing tzc\n");
			}
			else if (err==EPERM) {
				purple_debug_error("zephyr","purple didn't have permission to kill tzc\n");
			}
			else {
				purple_debug_error("zephyr","miscellaneous error while attempting to close tzc\n");
			}
		}
	}
}
