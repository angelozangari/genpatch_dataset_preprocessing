}
static void zephyr_set_status(PurpleAccount *account, PurpleStatus *status) {
	size_t len;
	size_t result;
	zephyr_account *zephyr = purple_account_get_connection(account)->proto_data;
	PurpleStatusPrimitive primitive = purple_status_type_get_primitive(purple_status_get_type(status));
	if (zephyr->away) {
		g_free(zephyr->away);
		zephyr->away=NULL;
	}
	if (primitive == PURPLE_STATUS_AWAY) {
		zephyr->away = g_strdup(purple_status_get_attr_string(status,"message"));
	}
	else if (primitive == PURPLE_STATUS_AVAILABLE) {
		if (use_zeph02(zephyr)) {
			ZSetLocation(zephyr->exposure);
		}
		else {
			char *zexpstr = g_strdup_printf("((tzcfodder . set-location) (hostname . \"%s\") (exposure . \"%s\"))\n",zephyr->ourhost,zephyr->exposure);
			len = strlen(zexpstr);
			result = write(zephyr->totzc[ZEPHYR_FD_WRITE],zexpstr,len);
			if (result != len) {
				purple_debug_error("zephyr", "Unable to write message: %s\n", g_strerror(errno));
			}
			g_free(zexpstr);
		}
	}
	else if (primitive == PURPLE_STATUS_INVISIBLE) {
		/* XXX handle errors */
		if (use_zeph02(zephyr)) {
			ZSetLocation(EXPOSE_OPSTAFF);
		} else {
			char *zexpstr = g_strdup_printf("((tzcfodder . set-location) (hostname . \"%s\") (exposure . \"%s\"))\n",zephyr->ourhost,EXPOSE_OPSTAFF);
			len = strlen(zexpstr);
			result = write(zephyr->totzc[ZEPHYR_FD_WRITE],zexpstr,len);
			if (result != len) {
				purple_debug_error("zephyr", "Unable to write message: %s\n", g_strerror(errno));
			}
			g_free(zexpstr);
		}
	}
}
