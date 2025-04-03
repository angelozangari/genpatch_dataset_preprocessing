}
static void hbn_cb(GSList *hosts, gpointer data, const char *error_message) {
	if(!hosts || !hosts->data) {
		nattype.status = PURPLE_STUN_STATUS_UNDISCOVERED;
		nattype.lookup_time = time(NULL);
		do_callbacks();
		return;
	}
	if (!purple_network_listen_range(12108, 12208, SOCK_DGRAM, hbn_listen_cb, hosts)) {
		while (hosts) {
			hosts = g_slist_delete_link(hosts, hosts);
			g_free(hosts->data);
			hosts = g_slist_delete_link(hosts, hosts);
		}
		nattype.status = PURPLE_STUN_STATUS_UNKNOWN;
		nattype.lookup_time = time(NULL);
		do_callbacks();
		return;
	}
}
}
static void hbn_cb(GSList *hosts, gpointer data, const char *error_message) {
	if(!hosts || !hosts->data) {
		nattype.status = PURPLE_STUN_STATUS_UNDISCOVERED;
		nattype.lookup_time = time(NULL);
		do_callbacks();
		return;
	}
	if (!purple_network_listen_range(12108, 12208, SOCK_DGRAM, hbn_listen_cb, hosts)) {
		while (hosts) {
			hosts = g_slist_delete_link(hosts, hosts);
			g_free(hosts->data);
			hosts = g_slist_delete_link(hosts, hosts);
		}
		nattype.status = PURPLE_STUN_STATUS_UNKNOWN;
		nattype.lookup_time = time(NULL);
		do_callbacks();
		return;
	}
}
