}
static void hbn_listen_cb(int fd, gpointer data) {
	GSList *hosts = data;
	struct stun_conn *sc;
	static struct stun_header hdr_data;
	if(fd < 0) {
		nattype.status = PURPLE_STUN_STATUS_UNKNOWN;
		nattype.lookup_time = time(NULL);
		do_callbacks();
		return;
	}
	sc = g_new0(struct stun_conn, 1);
	sc->fd = fd;
	sc->addr.sin_family = AF_INET;
	sc->addr.sin_port = htons(purple_network_get_port_from_fd(fd));
	sc->addr.sin_addr.s_addr = INADDR_ANY;
	sc->incb = purple_input_add(fd, PURPLE_INPUT_READ, reply_cb, sc);
	hosts = g_slist_delete_link(hosts, hosts);
	memcpy(&(sc->addr), hosts->data, sizeof(struct sockaddr_in));
	g_free(hosts->data);
	hosts = g_slist_delete_link(hosts, hosts);
	while (hosts) {
		hosts = g_slist_delete_link(hosts, hosts);
		g_free(hosts->data);
		hosts = g_slist_delete_link(hosts, hosts);
	}
	hdr_data.type = htons(MSGTYPE_BINDINGREQUEST);
	hdr_data.len = 0;
	hdr_data.transid[0] = rand();
	hdr_data.transid[1] = ntohl(((int)'g' << 24) + ((int)'a' << 16) + ((int)'i' << 8) + (int)'m');
	hdr_data.transid[2] = rand();
	hdr_data.transid[3] = rand();
	if(sendto(sc->fd, &hdr_data, sizeof(struct stun_header), 0,
			(struct sockaddr *)&(sc->addr),
			sizeof(struct sockaddr_in)) < (gssize)sizeof(struct stun_header)) {
		nattype.status = PURPLE_STUN_STATUS_UNKNOWN;
		nattype.lookup_time = time(NULL);
		do_callbacks();
		close_stun_conn(sc);
		return;
	}
	sc->test = 1;
	sc->packet = &hdr_data;
	sc->packetsize = sizeof(struct stun_header);
	sc->timeout = purple_timeout_add(500, (GSourceFunc) timeoutfunc, sc);
}
