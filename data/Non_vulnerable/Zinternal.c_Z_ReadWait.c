 */
Code_t Z_ReadWait()
{
    register struct _Z_InputQ *qptr;
    ZNotice_t notice;
    ZPacket_t packet;
    struct sockaddr_in olddest, from;
    int packet_len, zvlen, part, partof;
    socklen_t from_len;
    char *slash;
    Code_t retval;
    fd_set fds;
    struct timeval tv;
    if (ZGetFD() < 0)
	return (ZERR_NOPORT);
    FD_ZERO(&fds);
    FD_SET(ZGetFD(), &fds);
    tv.tv_sec = 60;
    tv.tv_usec = 0;
    if (select(ZGetFD() + 1, &fds, NULL, NULL, &tv) < 0)
      return (errno);
    if (!FD_ISSET(ZGetFD(), &fds))
      return ETIMEDOUT;
    from_len = sizeof(struct sockaddr_in);
    packet_len = recvfrom(ZGetFD(), packet, sizeof(packet) - 1, 0,
			  (struct sockaddr *)&from, &from_len);
    if (packet_len < 0)
	return (errno);
    if (!packet_len)
	return (ZERR_EOF);
    packet[packet_len] = '\0';
    /* Ignore obviously non-Zephyr packets. */
    zvlen = sizeof(ZVERSIONHDR) - 1;
    if (packet_len < zvlen || memcmp(packet, ZVERSIONHDR, zvlen) != 0) {
	Z_discarded_packets++;
	return (ZERR_NONE);
    }
    /* Parse the notice */
    if ((retval = ZParseNotice(packet, packet_len, &notice)) != ZERR_NONE)
	return (retval);
    /*
     * If we're not a server and the notice is of an appropriate kind,
     * send back a CLIENTACK to whoever sent it to say we got it.
     */
    if (!__Zephyr_server) {
	if (notice.z_kind != HMACK && notice.z_kind != SERVACK &&
	    notice.z_kind != SERVNAK && notice.z_kind != CLIENTACK) {
	    ZNotice_t tmpnotice;
	    ZPacket_t pkt;
	    int len;
	    tmpnotice = notice;
	    tmpnotice.z_kind = CLIENTACK;
	    tmpnotice.z_message_len = 0;
	    olddest = __HM_addr;
	    __HM_addr = from;
	    if ((retval = ZFormatSmallRawNotice(&tmpnotice, pkt, &len))
		!= ZERR_NONE)
		return(retval);
	    if ((retval = ZSendPacket(pkt, len, 0)) != ZERR_NONE)
		return (retval);
	    __HM_addr = olddest;
	}
	if (find_or_insert_uid(&notice.z_uid, notice.z_kind))
	    return(ZERR_NONE);
	/* Check authentication on the notice. */
	notice.z_checked_auth = ZCheckAuthentication(&notice, &from);
    }
    /*
     * Parse apart the z_multinotice field - if the field is blank for
     * some reason, assume this packet stands by itself.
     */
    slash = strchr(notice.z_multinotice, '/');
    if (slash) {
	part = atoi(notice.z_multinotice);
	partof = atoi(slash+1);
	if (part > partof || partof == 0) {
	    part = 0;
	    partof = notice.z_message_len;
	}
    }
    else {
	part = 0;
	partof = notice.z_message_len;
    }
    /* Too big a packet...just ignore it! */
    if (partof > Z_MAXNOTICESIZE)
	return (ZERR_NONE);
    /*
     * If we aren't a server and we can find a notice in the queue
     * with the same multiuid field, insert the current fragment as
     * appropriate.
     */
    switch (notice.z_kind) {
    case SERVACK:
    case SERVNAK:
	/* The SERVACK and SERVNAK replies shouldn't be reassembled
	   (they have no parts).  Instead, we should hold on to the reply
	   ONLY if it's the first part of a fragmented message, i.e.
	   multi_uid == uid.  This allows programs to wait for the uid
	   of the first packet, and get a response when that notice
	   arrives.  Acknowledgements of the other fragments are discarded
	   (XXX we assume here that they all carry the same information
	   regarding failure/success)
	 */
	if (!__Zephyr_server &&
	    !ZCompareUID(&notice.z_multiuid, &notice.z_uid))
	    /* they're not the same... throw away this packet. */
	    return(ZERR_NONE);
	/* fall thru & process it */
    default:
	/* for HMACK types, we assume no packet loss (local loopback
	   connections).  The other types can be fragmented and MUST
	   run through this code. */
	if (!__Zephyr_server && (qptr = Z_SearchQueue(&notice.z_multiuid,
						      notice.z_kind))) {
	    /*
	     * If this is the first fragment, and we haven't already
	     * gotten a first fragment, grab the header from it.
	     */
	    if (part == 0 && !qptr->header) {
		qptr->header_len = packet_len-notice.z_message_len;
		qptr->header = (char *) malloc((unsigned) qptr->header_len);
		if (!qptr->header)
		    return (ENOMEM);
		(void) memcpy(qptr->header, packet, qptr->header_len);
	    }
	    return (Z_AddNoticeToEntry(qptr, &notice, part));
	}
    }
    /*
     * We'll have to create a new entry...make sure the queue isn't
     * going to get too big.
     */
    if (__Q_Size+(__Zephyr_server ? notice.z_message_len : partof) > Z_MAXQUEUESIZE)
	return (ZERR_NONE);
    /*
     * This is a notice we haven't heard of, so create a new queue
     * entry for it and zero it out.
     */
    qptr = (struct _Z_InputQ *)malloc(sizeof(struct _Z_InputQ));
    if (!qptr)
	return (ENOMEM);
    (void) memset((char *)qptr, 0, sizeof(struct _Z_InputQ));
    /* Insert the entry at the end of the queue */
    qptr->next = NULL;
    qptr->prev = __Q_Tail;
    if (__Q_Tail)
	__Q_Tail->next = qptr;
    __Q_Tail = qptr;
    if (!__Q_Head)
	__Q_Head = qptr;
    /* Copy the from field, multiuid, kind, and checked authentication. */
    qptr->from = from;
    qptr->uid = notice.z_multiuid;
    qptr->kind = notice.z_kind;
    qptr->auth = notice.z_checked_auth;
    /*
     * If this is the first part of the notice, we take the header
     * from it.  We only take it if this is the first fragment so that
     * the Unique ID's will be predictable.
     *
     * If a Zephyr Server, we always take the header.
     */
    if (__Zephyr_server || part == 0) {
	qptr->header_len = packet_len-notice.z_message_len;
	qptr->header = (char *) malloc((unsigned) qptr->header_len);
	if (!qptr->header)
	    return ENOMEM;
	(void) memcpy(qptr->header, packet, qptr->header_len);
    }
    /*
     * If this is not a fragmented notice, then don't bother with a
     * hole list.
     * If we are a Zephyr server, all notices are treated as complete.
     */
    if (__Zephyr_server || (part == 0 && notice.z_message_len == partof)) {
	__Q_CompleteLength++;
	qptr->holelist = (struct _Z_Hole *) 0;
	qptr->complete = 1;
	/* allocate a msg buf for this piece */
	if (notice.z_message_len == 0)
	    qptr->msg = 0;
	else if (!(qptr->msg = (char *) malloc((unsigned) notice.z_message_len)))
	    return(ENOMEM);
	else
	    (void) memcpy(qptr->msg, notice.z_message, notice.z_message_len);
	qptr->msg_len = notice.z_message_len;
	__Q_Size += notice.z_message_len;
	qptr->packet_len = qptr->header_len+qptr->msg_len;
	if (!(qptr->packet = (char *) malloc((unsigned) qptr->packet_len)))
	    return (ENOMEM);
	(void) memcpy(qptr->packet, qptr->header, qptr->header_len);
	if(qptr->msg)
	    (void) memcpy(qptr->packet+qptr->header_len, qptr->msg,
			   qptr->msg_len);
	return (ZERR_NONE);
    }
    /*
     * We know how long the message is going to be (this is better
     * than IP fragmentation...), so go ahead and allocate it all.
     */
    if (!(qptr->msg = (char *) malloc((unsigned) partof)) && partof)
	return (ENOMEM);
    qptr->msg_len = partof;
    __Q_Size += partof;
    /*
     * Well, it's a fragmented notice...allocate a hole list and
     * initialize it to the full packet size.  Then insert the
     * current fragment.
     */
    if (!(qptr->holelist = (struct _Z_Hole *)
	  malloc(sizeof(struct _Z_Hole))))
	return (ENOMEM);
    qptr->holelist->next = (struct _Z_Hole *) 0;
    qptr->holelist->first = 0;
    qptr->holelist->last = partof-1;
    return (Z_AddNoticeToEntry(qptr, &notice, part));
}
