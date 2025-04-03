#ifdef NOTYET
static void do_test2(struct stun_conn *sc) {
	struct stun_change data;
	data.hdr.type = htons(0x0001);
	data.hdr.len = 0;
	data.hdr.transid[0] = rand();
	data.hdr.transid[1] = ntohl(((int)'g' << 24) + ((int)'a' << 16) + ((int)'i' << 8) + (int)'m');
	data.hdr.transid[2] = rand();
	data.hdr.transid[3] = rand();
	data.attrib.type = htons(0x003);
	data.attrib.len = htons(4);
	data.value[3] = 6;
	sc->packet = (struct stun_header*)&data;
	sc->packetsize = sizeof(struct stun_change);
	sc->retry = 0;
	sc->test = 2;
	sendto(sc->fd, sc->packet, sc->packetsize, 0, (struct sockaddr *)&(sc->addr), sizeof(struct sockaddr_in));
	sc->timeout = purple_timeout_add(500, (GSourceFunc) timeoutfunc, sc);
}
