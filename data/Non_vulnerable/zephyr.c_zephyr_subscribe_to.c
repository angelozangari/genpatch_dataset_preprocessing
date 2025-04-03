#endif
static Code_t zephyr_subscribe_to(zephyr_account* zephyr, char* class, char *instance, char *recipient, char* galaxy) {
	size_t result;
	Code_t ret_val = -1;
	if (use_tzc(zephyr)) {
		/* ((tzcfodder . subscribe) ("class" "instance" "recipient")) */
		gchar *zsubstr = g_strdup_printf("((tzcfodder . subscribe) (\"%s\" \"%s\" \"%s\"))\n",class,instance,recipient);
		size_t len = strlen(zsubstr);
		result = write(zephyr->totzc[ZEPHYR_FD_WRITE],zsubstr,len);
		if (result != len) {
			purple_debug_error("zephyr", "Unable to write a message: %s\n", g_strerror(errno));
		} else {
			ret_val = ZERR_NONE;
		}
		g_free(zsubstr);
	}
	else {
		if (use_zeph02(zephyr)) {
			ZSubscription_t sub;
			sub.zsub_class = class;
			sub.zsub_classinst = instance;
			sub.zsub_recipient = recipient;
			ret_val = ZSubscribeTo(&sub,1,0);
		}
	}
	return ret_val;
}
