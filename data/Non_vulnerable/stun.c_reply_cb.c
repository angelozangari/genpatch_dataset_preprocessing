#endif
static void reply_cb(gpointer data, gint source, PurpleInputCondition cond) {
	struct stun_conn *sc = data;
	char buffer[65536];
	char *tmp;
	gssize len;
	struct in_addr in;
	struct stun_attrib *attrib;
	struct stun_header *hdr;
	struct ifconf ifc;
	struct ifreq *ifr;
	struct sockaddr_in *sinptr;
	memset(&in, 0, sizeof(in));
	len = recv(source, buffer, sizeof(buffer) - 1, 0);
	if (len < 0) {
		purple_debug_warning("stun", "unable to read stun response\n");
		return;
	}
	buffer[len] = '\0';
	if ((gsize)len < sizeof(struct stun_header)) {
		purple_debug_warning("stun", "got invalid response\n");
		return;
	}
	hdr = (struct stun_header*) buffer;
	if ((gsize)len != (ntohs(hdr->len) + sizeof(struct stun_header))) {
		purple_debug_warning("stun", "got incomplete response\n");
		return;
	}
	/* wrong transaction */
	if(hdr->transid[0] != sc->packet->transid[0]
			|| hdr->transid[1] != sc->packet->transid[1]
			|| hdr->transid[2] != sc->packet->transid[2]
			|| hdr->transid[3] != sc->packet->transid[3]) {
		purple_debug_warning("stun", "got wrong transid\n");
		return;
	}
	if(sc->test==1) {
		if (hdr->type != MSGTYPE_BINDINGRESPONSE) {
			purple_debug_warning("stun",
				"Expected Binding Response, got %d\n",
				hdr->type);
			return;
		}
		tmp = buffer + sizeof(struct stun_header);
		while((buffer + len) > (tmp + sizeof(struct stun_attrib))) {
			attrib = (struct stun_attrib*) tmp;
			tmp += sizeof(struct stun_attrib);
			if (!((buffer + len) > (tmp + ntohs(attrib->len))))
				break;
			if(attrib->type == htons(ATTRIB_MAPPEDADDRESS)
					&& ntohs(attrib->len) == 8) {
				char *ip;
				/* Skip the first unused byte,
				 * the family(1 byte), and the port(2 bytes);
				 * then read the 4 byte IPv4 address */
				memcpy(&in.s_addr, tmp + 4, 4);
				ip = inet_ntoa(in);
				if(ip)
					g_strlcpy(nattype.publicip, ip, sizeof(nattype.publicip));
			}
			tmp += ntohs(attrib->len);
		}
		purple_debug_info("stun", "got public ip %s\n", nattype.publicip);
		nattype.status = PURPLE_STUN_STATUS_DISCOVERED;
		nattype.type = PURPLE_STUN_NAT_TYPE_UNKNOWN_NAT;
		nattype.lookup_time = time(NULL);
		/* is it a NAT? */
		ifc.ifc_len = sizeof(buffer);
		ifc.ifc_req = (struct ifreq *) buffer;
		ioctl(source, SIOCGIFCONF, &ifc);
		tmp = buffer;
		while(tmp < buffer + ifc.ifc_len) {
			ifr = (struct ifreq *) tmp;
			tmp += sizeof(struct ifreq);
			if(ifr->ifr_addr.sa_family == AF_INET) {
				/* we only care about ipv4 interfaces */
				sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
				if(sinptr->sin_addr.s_addr == in.s_addr) {
					/* no NAT */
					purple_debug_info("stun", "no nat\n");
					nattype.type = PURPLE_STUN_NAT_TYPE_PUBLIC_IP;
				}
			}
		}
#ifndef NOTYET
		close_stun_conn(sc);
		do_callbacks();
#else
		purple_timeout_remove(sc->timeout);
		sc->timeout = 0;
		do_test2(sc);
	} else if(sc->test == 2) {
		close_stun_conn(sc);
		nattype.type = PURPLE_STUN_NAT_TYPE_FULL_CONE;
		do_callbacks();
#endif
	}
}
